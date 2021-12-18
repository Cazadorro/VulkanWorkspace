import matplotlib.pyplot as plt
import numpy as np
import numpy.ma as ma
from enum import IntEnum
from typing import Callable
from skimage.draw import line

"""
Create Your Own Lattice Boltzmann Simulation (With Python)
Philip Mocz (2020) Princeton Univeristy, @PMocz
Simulate flow past cylinder
for an isothermal fluid
"""

dir_count = 9


class LBM(IntEnum):
    IDX_NW = 0
    IDX_NM = 1
    IDX_NE = 2
    IDX_WW = 3
    IDX_MM = 4
    IDX_EE = 5
    IDX_SW = 6
    IDX_SM = 7
    IDX_SE = 8
    DIR_COUNT = 9


def calc_force_i(w, ux, uy, cx, cy, c_2, force_x, force_y):
    c_i_ab = np.zeros((2, 2))
    c_i_ab[0, 0] = cx * cx - c_2
    c_i_ab[0, 1] = cx * cy
    c_i_ab[1, 0] = cx * cy
    c_i_ab[1, 1] = cy * cy - c_2

    c_i = np.array([cx, cy])
    force = np.array([force_x, force_y])
    velocity = np.concatenate((ux[..., np.newaxis], uy[..., np.newaxis]), axis=2)
    velocity = velocity.reshape(-1, velocity.shape[-1])
    c_i_ab_array = np.repeat(c_i_ab.reshape(1, c_i_ab.shape[0], c_i_ab.shape[1]), ux.size, 0)
    ci_vel = np.einsum('ijk,ik->ij', c_i_ab_array,
                       velocity)
    return (w * np.dot(((c_i / c_2) + (ci_vel) / (c_2 * c_2)), force)).reshape(ux.shape)

def calc_force_i_2(w, cx,cy, force_xy):
    c_i = np.array([cx, cy])
    return (3.0/2.0) * w * np.dot(force_xy, c_i)

def calc_force_i_incompressible(w, cx, cy, c_2, force_x, force_y):
    c_i = np.array([cx, cy])
    force = np.array([force_x, force_y])
    return w * np.dot(c_i, force) / c_2


# in 2d, rotation axis can only be z, so sum up mass * distance to z for each cell should approximate moment of inertia.
# https://www.feynmanlectures.caltech.edu/I_19.html
# done with flood fill?
import itertools


def flood_select(initial_index, array: np.ndarray, selector: Callable):
    stack = []
    stack.append(initial_index)
    mask = ma.zeros(array.shape)
    offsets = [-1, 0, 1]
    offset_permutations = [x for x in itertools.product(*([offsets] * len(initial_index))) if np.sum(np.abs(x)) == 1]
    while len(stack) != 0:
        index = stack.pop()
        if selector(array, index):
            mask[index] = ma.masked
            for offset in offset_permutations:
                offset_index = index + offset
                stack.append(offset_index)
    return mask


def flood_select_masked(initial_index, array: np.ndarray):
    stack = []
    stack.append(initial_index)
    mask = ma.zeros(array.shape)
    offsets = [-1, 0, 1]
    offset_permutations = [x for x in itertools.product(*([offsets] * len(initial_index))) if np.sum(np.abs(x)) == 1]
    while len(stack) != 0:
        index = stack.pop()
        if array.mask[tuple(index)] != ma.masked:
            mask[tuple(index)] = ma.masked
            for offset in offset_permutations:
                offset_index = index + offset
                stack.append(offset_index)
    return mask


def calc_masked_border(pos_x, pos_y, map_shape):
    mask = ma.zeros(map_shape)
    assert (len(pos_x) == len(pos_y))
    for idx in range(len(pos_x)):
        begin_x = pos_x[idx]
        begin_y = pos_y[idx]
        end_x = pos_x[(idx + 1) % len(pos_x)]
        end_y = pos_y[(idx + 1) % len(pos_x)]
        rr, cc = line(begin_y, begin_x, end_y, end_x)
        for y, x in zip(rr, cc):
            mask[y, x] = ma.masked
    return mask


def calc_obj_info(initial_index, masked_border, mass):
    selected = flood_select_masked(initial_index, masked_border)
    indexes = np.argwhere(selected != None)
    centroid = np.sum(indexes, 0) / len(indexes)
    offset_indexes = indexes - centroid
    offset_distance = np.linalg.norm(offset_indexes, axis=len(offset_indexes.shape) - 1)
    offset_distance_2 = offset_distance * offset_distance
    moment_of_inertia = mass * np.sum(offset_distance_2)
    area = len(indexes)
    return centroid, moment_of_inertia, area


def calc_2d_enclosed_centroid_moi(initial_index, pos_x, pos_y, map_shape, mass):
    masked_border = calc_masked_border(pos_x, pos_y, map_shape)
    return calc_obj_info(initial_index, masked_border, mass)


def border_iterate(pos_x, pos_y, transform, map_shape):
    assert (len(pos_x) == len(pos_y))
    for idx in range(len(pos_x)):
        begin_x = pos_x[idx]
        begin_y = pos_y[idx]
        end_x = pos_x[(idx + 1) % len(pos_x)]
        end_y = pos_y[(idx + 1) % len(pos_x)]
        begin = np.array([begin_x, begin_y, 1])
        end = np.array([end_x, end_y, 1])
        begin = transform @ begin
        end = transform @ end
        rr, cc = line(int(begin[1]), int(begin[0]), int(end[1]), int(end[0]))
        # rr = int(rr)
        # cc = int(cc)
        for y, x in zip(rr, cc):
            y = int(y)
            x = int(x)
            if y >= 0 and y < map_shape[0] and x >= 0 and x < map_shape[1]:
                # row column
                yield (y, x)


def calc_force_at_nodes(nodes, node_initial_state, transform, average_node_distance, dx, spring_strength):
    # transform every value in initial state
    for i in range(len(node_initial_state)):
        node_initial_state[i] = (transform @ np.array([node_initial_state[i][0],node_initial_state[i][1],1])[0:2])
    diff = nodes - node_initial_state
    return spring_strength * (average_node_distance/dx) * diff

def main():
    """ Finite Volume simulation """

    # Simulation parameters
    Nx = 64  # resolution x-dir
    Ny = 32  # resolution y-dir
    rho0 = 100  # average density
    tau = 0.6  # collision timescale
    dt = 1.0
    tau = tau + dt / 2
    c_2 = 1 / 3
    Nt = 10000  # number of timesteps
    plotRealTime = True  # switch on for plotting as the simulation goes along

    # Lattice speeds / weights
    NL = 9
    idxs = np.arange(NL)

    cxs = np.array([0, 0, 1, 1, 1, 0, -1, -1, -1])
    cys = np.array([0, 1, 1, 0, -1, -1, -1, 0, 1])

    cxs = np.array([-1, 0, 1, -1, 0, 1, -1, 0, 1])
    cys = np.array([-1, -1, -1, 0, 0, 0, 1, 1, 1])
    cardinal_weight = 1.0 / 9.0
    ordinal_weight = 1.0 / 36.0
    middle_weight = 4.0 / 9.0

    opposite_idxs = np.array([
        LBM.IDX_SE, LBM.IDX_SM, LBM.IDX_SW,
        LBM.IDX_EE, LBM.IDX_MM, LBM.IDX_WW,
        LBM.IDX_NE, LBM.IDX_NM, LBM.IDX_NW
    ])
    weights = np.array([4 / 9, 1 / 9, 1 / 36, 1 / 9, 1 / 36, 1 / 9, 1 / 36, 1 / 9, 1 / 36])  # sums to 1
    weights = np.array([
        ordinal_weight, cardinal_weight, ordinal_weight,
        cardinal_weight, middle_weight, cardinal_weight,
        ordinal_weight, cardinal_weight, ordinal_weight
    ])

    force_x = 0.0
    force_y = 0.1

    # Initial Conditions
    F = np.ones((NL, Ny, Nx))  # * rho0 / NL
    np.random.seed(42)
    # F += 0.01 * np.random.randn(NL, Ny, Nx)

    X, Y = np.meshgrid(range(Nx), range(Ny))
    F[LBM.IDX_EE, :, :] += 2.0 * (1 + 0.2 * np.cos(2 * np.pi * X / Nx * 4))
    rho = np.sum(F, 0)
    for i in idxs:
        F[i, :, :] *= rho0 / rho

    # Cylinder boundary
    X, Y = np.meshgrid(range(Nx), range(Ny))
    cylinder = (X - Nx / 4) ** 2 + (Y - Ny / 2) ** 2 < (Ny / 4) ** 2
    cylinder[0, :] = True
    # Prep figure
    fig = plt.figure(figsize=(4, 2), dpi=80)

    vertex_x = [int(Nx * 2 / 4), int(Nx * 3 / 4), int(Nx * 3 / 4), int(Nx * 2 / 4)]
    vertex_y = [int(Ny * 2 / 4), int(Ny * 2 / 4), int(Ny * 3 / 4), int(Ny * 3 / 4)]

    initial_point = [int((int(Ny * 2 / 4) + int(Ny * 3 / 4)) / 2), int((int(Nx * 2 / 4) + int(Nx * 3 / 4)) / 2)]

    mass = 1.0
    # moment of inertia = moi.
    map_shape = (Ny, Nx)
    centroid, moi, area = calc_2d_enclosed_centroid_moi(initial_point, vertex_x, vertex_y, map_shape, mass)
    total_mass = area * mass
    position = np.array(centroid)
    position = np.array([0.0, 0.0])
    velocity = np.array([0.0, 0.0])

    transform = np.eye(3, 3)

    angle = 0.0
    angular_velocity = 0.0

    # Simulation Main Loop
    for it in range(Nt):
        print(it)

        # Drift/Advection
        for i, cx, cy in zip(idxs, cxs, cys):
            F[i, :, :] = np.roll(F[i, :, :], cx, axis=1)
            F[i, :, :] = np.roll(F[i, :, :], cy, axis=0)

        # Set reflective boundaries
        bndryF = F[:, cylinder]
        bndryF = bndryF[opposite_idxs, :]
        # Apply boundary
        F[:, cylinder] = bndryF
        forces_incompress = np.zeros(F.shape)
        for i, cx, cy, w in zip(idxs, cxs, cys, weights):
            forces_incompress[i, :, :] = calc_force_i_incompressible(w, cx, cy, c_2, force_x, force_y)
        # Calculate fluid variables
        # rho = np.sum(F, 0) + (dt / 2.0) * np.sum(forces_incompress, 0)
        # ux = np.sum(F * cxs.reshape(9, 1, 1) + (dt / 2.0) * (forces_incompress * cxs.reshape(9, 1, 1)), 0) / rho
        # uy = np.sum(F * cys.reshape(9, 1, 1) + (dt / 2.0) * (forces_incompress * cys.reshape(9, 1, 1)), 0) / rho

        rho = np.sum(F, 0)
        ux = np.sum(F * cxs.reshape(9, 1, 1), 0) / rho
        uy = np.sum(F * cys.reshape(9, 1, 1), 0) / rho

        # rho = np.sum(F, 0)
        # ux = np.sum(F * cxs.reshape(9,1,1), 0) / rho + (force_x * dt / (2 * rho))
        # uy = np.sum(F * cys.reshape(9,1,1), 0) / rho + (force_y * dt / (2 * rho))

        # Apply Collision
        Feq = np.zeros(F.shape)
        forces = np.zeros(F.shape)
        forces = np.array([calc_force_i_2(w,cx,cy,np.array([force_x,force_y])) for cx, cy, w in zip(cxs, cys, weights)]).reshape(9, 1, 1)
        for i, cx, cy, w in zip(idxs, cxs, cys, weights):
            # forces[i, :, :] = calc_force_i(w, ux, uy, cx, cy, c_2, force_x, force_y)

            f_eq = rho * w * (
                    1 + 3 * (cx * ux + cy * uy) + 9 * (cx * ux + cy * uy) ** 2 / 2 - 3 * (ux ** 2 + uy ** 2) / 2)
            # f_eq = rho * w + w *rho0* (
            #         1 + 3 * (cx * ux + cy * uy) + 9 * (cx * ux + cy * uy) ** 2 / 2 - 3 * (ux ** 2 + uy ** 2) / 2)

            Feq[i, :, :] = f_eq
        F += -(dt / tau) * (F - Feq) + forces

        if False:
            indexes = [(x, y) for x, y in border_iterate(vertex_x, vertex_y, transform, map_shape)]
            indexes = tuple(np.array(indexes).T)
            FNet = np.array([0, 0])
            dx = 0.1
            if indexes:
                OldF = F[:, indexes[0], indexes[1]]
                dj = np.empty((OldF.shape[0], OldF.shape[1], 2))
                for i, cx, cy, w in zip(idxs, cxs, cys, weights):
                    F[i, indexes[0], indexes[1]] += (2.0 / c_2) * w * np.dot(np.array([cx, cy]), velocity) * rho[
                        indexes[0], indexes[1]]
                    dj[i, :, :] = np.multiply.outer(F[i, indexes[0], indexes[1]] - OldF[i, :], np.array([cx, cy]))
                # dj = F[:, indexes[0], indexes[1]] - OldF
                FNet = np.sum(dj, (0, 1))
            else:
                FNet = np.array([0.0, 0.0])
            FNet += np.array([force_x, -force_y])
            FNet *= dx/dt
            position += velocity * dt
            print(position)
            velocity += (FNet / total_mass) * 0.01 * dt
            transform = np.array([
                [1, 0, position[0]],
                [0, 1, position[1]]
            ])


        # plot in real time - color 1/2 particles blue, other half red
        if (plotRealTime and (it % 10) == 0) or (it == Nt - 1):
            plt.cla()
            # ux[cylinder] = 0
            # uy[cylinder] = 0
            vorticity = (np.roll(ux, -1, axis=0) - np.roll(ux, 1, axis=0)) - (
                    np.roll(uy, -1, axis=1) - np.roll(uy, 1, axis=1))
            # vorticity *= 10.0
            vorticity = np.sqrt(ux ** 2 + uy ** 2)
            # vorticity*=1000
            vorticity[cylinder] = np.nan
            for x, y in zip(vertex_x, vertex_y):
                temp = transform @ np.array([x, y, 1])
                x = int(temp[0])
                y = int(temp[1])
                if x >= 0 and y >= 0 and x < map_shape[1] and y < map_shape[0]:
                    vorticity[y, x] = np.nan
            cmap = plt.cm.bwr
            cmap.set_bad('black')
            plt.imshow(vorticity, cmap='bwr')
            plt.clim(-.1, .1)
            ax = plt.gca()
            ax.invert_yaxis()
            ax.get_xaxis().set_visible(False)
            ax.get_yaxis().set_visible(False)
            ax.set_aspect('equal')
            plt.pause(0.001)

    # Save figure
    plt.savefig('latticeboltzmann.png', dpi=240)
    plt.show()

    return 0


if __name__ == "__main__":
    main()
