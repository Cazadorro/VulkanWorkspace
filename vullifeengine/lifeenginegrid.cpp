//
// Created by Bolt on 6/25/2023.
//

#include "lifeenginegrid.h"
#include <uul/math.h>
#include <uul/assert.h>
#include <uul/indexing.h>
#include <uul/enumflags.h>
#include <uul/sortingnetworks.h>
#include <pcg_random.hpp>
#include <pcg_extras.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/drop.hpp>
#include <random>
#include <functional>

le::CellType le::Grid::cell_type_at(std::uint32_t x, std::uint32_t y) const {
    return m_cell_type_grid[linear_index(x, y)];
}

std::uint8_t le::Grid::cell_argument_a_at(std::uint32_t x, std::uint32_t y) const {
    return m_cell_argument_a_grid[linear_index(x, y)];
}

std::uint8_t le::Grid::cell_argument_b_at(std::uint32_t x, std::uint32_t y) const {
    return m_cell_argument_b_grid[linear_index(x, y)];
}

std::uint8_t le::Grid::cell_health_at(std::uint32_t x, std::uint32_t y) const {
    return m_cell_health_grid[linear_index(x, y)];
}

std::uint64_t le::Grid::cell_id_at(std::uint32_t x, std::uint32_t y) const {
    return m_cell_id_grid[linear_index(x, y)];
}

std::uint32_t le::Grid::meat_at(std::uint32_t x, std::uint32_t y) const {
    return m_meat_energy_grid[linear_index(x, y)];
}

std::uint32_t le::Grid::detritus_at(std::uint32_t x, std::uint32_t y) const {
    return m_detritus_energy_grid[linear_index(x, y)];
}

le::CellType &le::Grid::cell_type_at(std::uint32_t x, std::uint32_t y) {
    return m_cell_type_grid[linear_index(x, y)];
}

std::uint8_t &le::Grid::cell_argument_a_at(std::uint32_t x, std::uint32_t y) {
    return m_cell_argument_a_grid[linear_index(x, y)];
}

std::uint8_t &le::Grid::cell_argument_b_at(std::uint32_t x, std::uint32_t y) {
    return m_cell_argument_b_grid[linear_index(x, y)];
}

std::uint64_t &le::Grid::cell_id_at(std::uint32_t x, std::uint32_t y) {
    return m_cell_id_grid[linear_index(x, y)];
}

std::uint32_t &le::Grid::meat_at(std::uint32_t x, std::uint32_t y) {
    return m_meat_energy_grid[linear_index(x, y)];
}

std::uint32_t &le::Grid::detritus_at(std::uint32_t x, std::uint32_t y) {
    return m_detritus_energy_grid[linear_index(x, y)];
}


std::uint8_t &le::Grid::cell_health_at(std::uint32_t x, std::uint32_t y) {
    return m_cell_health_grid[linear_index(x, y)];
}

std::uint8_t le::Grid::cell_output_a_grid(std::uint32_t x, std::uint32_t y) const {
    return m_cell_output_a_grid[linear_index(x, y)];
}

std::uint8_t &le::Grid::cell_output_a_grid(std::uint32_t x, std::uint32_t y) {
    return m_cell_output_a_grid[linear_index(x, y)];
}

std::uint32_t le::Grid::width() const {
    return m_width;
}

std::uint32_t le::Grid::height() const {
    return m_height;
}

std::uint64_t le::Grid::size() const {
    return width() * height();
}

std::size_t le::Grid::linear_index(std::uint32_t x, std::uint32_t y) const {
    return y * width() + x;
}

le::Grid::Grid(std::uint32_t width, std::uint32_t height) :
        m_width(width), m_height(height),
        m_cell_id_grid(m_width * m_height, consts::null_cell_id),
        m_cell_type_grid(m_width * m_height, CellType::Void),
        m_meat_energy_grid(m_width * m_height, 0),
        m_cell_argument_a_grid(m_width * m_height, 0),
        m_cell_argument_b_grid(m_width * m_height, 0),
        m_cell_health_grid(m_width * m_height, 0),
        m_cell_output_a_grid(m_width * m_height, 0) {

}

void le::Grid::fill_empty_organism_list() {
    m_empty_organism_list.clear();

    for (auto [index, current_phenotype]: ranges::views::enumerate(m_phenotypes)) {
        if (current_phenotype.is_dead()) {
            m_empty_organism_list.push_back(static_cast<std::uint32_t>(index));
        }
    }
}


std::uint64_t le::Grid::add_organism(le::Phenotype phenotype, le::Genome genome) {
    //need to figure out last elements that were availible to remove?

    //opposite skip field,
    //skip elements
    //fastest would just be a priority queue?
    //or organism can only be added in beginnning, go over entire list, create list of places to be added
    //
//    for(auto [index,current_phenotype] : ranges::views::enumerate(m_phenotypes)){
//        if(current_phenotype.is_dead()){
//            current_phenotype = phenotype;
//            return index;
//        }
//    }
    if (m_empty_organism_list.empty()) {
        std::uint64_t index = m_phenotypes.size();
        m_phenotypes.push_back(phenotype);
        m_genomes.push_back(std::move(genome));
        return index;
    } else {
        //would need to ring buffer it? insertion sort into?
        auto organism_idx = m_empty_organism_list.back();
        m_phenotypes[organism_idx] = phenotype;
        m_genomes[organism_idx] = genome;
        m_empty_organism_list.pop_back();
    }
}

std::uint64_t le::Grid::organism_count() const {
    return m_phenotypes.size();
}

const le::Phenotype &le::Grid::get_phenotype(std::uint64_t cell_id) const {
    return m_phenotypes[cell_id];
}

le::Phenotype &le::Grid::get_phenotype(std::uint64_t cell_id) {
    return m_phenotypes[cell_id];
}

void le::Grid::kill_cell_at(std::uint32_t x, std::uint32_t y) {
    auto dead_cell_type = cell_type_at(x, y);
    auto previous_cell_id = cell_id_at(x, y);
    UUL_ASSERT(get_phenotype(previous_cell_id).body_segment_count > 0);
    auto energy_lost =
            get_phenotype(previous_cell_id).energy_accumulation / get_phenotype(previous_cell_id).body_segment_count;
    get_phenotype(previous_cell_id).energy_accumulation -= energy_lost;
    get_phenotype(previous_cell_id).body_segment_count -= 1;
    meat_at(x, y) = consts::cell_death_cost + energy_lost;
    clear_cell_at(x,y);
}

void le::Grid::kill_organism(std::uint64_t organism_index) {
    for_every_pixel_in_organism(organism_index, [this](std::uint32_t x, std::uint32_t y) {
        kill_cell_at(x,y);
    });
}

void le::Grid::for_every_pixel_in_organism(std::uint64_t organism_index,
                                           const std::function<void(std::uint32_t, std::uint32_t)> &function) {

    const auto phenotype = m_phenotypes[organism_index];
    auto y_begin = phenotype.origin_y;
    auto y_end = phenotype.origin_y + phenotype.height;
    auto x_begin = phenotype.origin_x;
    auto x_end = phenotype.origin_x + phenotype.width;
    for(auto y : ranges::views::ints(y_begin, y_end)){
        for(auto x : ranges::views::ints(x_begin, x_end)){
            if(cell_id_at(x,y) == organism_index){
                function(x,y);
            }
        }
    }
}

void le::Grid::clear_cell_at(std::uint32_t x, std::uint32_t y) {
    cell_id_at(x, y) = consts::null_cell_id;
    cell_type_at(x, y) = CellType::Void;
    cell_argument_a_at(x, y) = 0;
    cell_argument_b_at(x, y) = 0;
    cell_output_a_grid(x, y) = 0;
    cell_health_at(x, y) = 0;
}

bool le::Grid::move_cell_to(glm::uvec2 current_pos, glm::uvec2 final_pos) {

    //TODO figure out what to do with leftover outputs? clear them?
    cell_id_at(final_pos.x, final_pos.y) = cell_id_at(current_pos.x, current_pos.y);
    cell_type_at(final_pos.x, final_pos.y) = cell_type_at(current_pos.x, current_pos.y);
    cell_argument_a_at(final_pos.x, final_pos.y) = cell_argument_a_at(current_pos.x, current_pos.y);
    cell_argument_b_at(final_pos.x, final_pos.y) = cell_argument_b_at(current_pos.x, current_pos.y);
    cell_output_a_grid(final_pos.x, final_pos.y) = cell_output_a_grid(current_pos.x, current_pos.y);
    cell_health_at(final_pos.x, final_pos.y) = cell_health_at(current_pos.x, current_pos.y);
    clear_cell_at(current_pos.x, current_pos.y);
}

uul::dim2d<std::uint32_t> le::Grid::dim() const {
    return {width(), height()};
}


le::Genome le::mutate(const le::Genome &genome) {
    //TODO add copy section of genome?
    std::vector<Gene> new_genes;
    new_genes.reserve(genome.genes.size());
    pcg32 rng(pcg_extras::seed_seq_from<std::random_device>{});
    std::uniform_real_distribution<double> real_dist(0.0, 1.0);
    std::normal_distribution<double> evolution_dist(0.0, genome.evolution_evolution_rate);
    std::normal_distribution<double> modify_a_dist(0.0, genome.modify_a_evolution_rate);
    std::normal_distribution<double> modify_b_dist(0.0, genome.modify_b_evolution_rate);
    std::uniform_int_distribution<std::uint32_t> cell_type_dist(0u, cell_type_end - 1);
    std::uniform_int_distribution<std::uint32_t> genome_command_dist(0u, genome_command_end - 1);
    std::uniform_int_distribution<std::uint32_t> argument_dist(0u, 255u);

    for (auto gene: genome.genes) {
        auto new_gene = gene;
        if (real_dist(rng) < genome.insert_evolution_rate) {
            new_genes.emplace_back(
                    static_cast<CellType>(cell_type_dist(rng)),
                    static_cast<GenomeCommand>(genome_command_dist(rng)),
                    static_cast<std::uint8_t>(argument_dist(rng)),
                    static_cast<std::uint8_t>(argument_dist(rng))
            );
        }
        if (real_dist(rng) < genome.change_evolution_rate) {
            new_gene = Gene{
                    static_cast<CellType>(cell_type_dist(rng)),
                    static_cast<GenomeCommand>(genome_command_dist(rng)),
                    static_cast<std::uint8_t>(argument_dist(rng)),
                    static_cast<std::uint8_t>(argument_dist(rng))
            };
        }

        //TODO change to chance and random value based?
        auto cell_type_argument_a = new_gene.cell_type_argument_a / 255.0;
        cell_type_argument_a = std::clamp(cell_type_argument_a + modify_a_dist(rng), 0.0, 1.0);
        new_gene.cell_type_argument_a = static_cast<std::uint8_t>(cell_type_argument_a * 255.0);

        auto cell_type_argument_b = new_gene.cell_type_argument_b / 255.0;
        cell_type_argument_b = std::clamp(cell_type_argument_b + modify_b_dist(rng), 0.0, 1.0);
        new_gene.cell_type_argument_b = static_cast<std::uint8_t>(cell_type_argument_b * 255.0);

        if (real_dist(rng) < genome.delete_evolution_rate) {

        } else {
            new_genes.push_back(new_gene);
        }
    }
    auto evolution_evolution_rate = std::clamp(genome.evolution_evolution_rate + evolution_dist(rng), 0.0, 1.0);
    auto insert_evolution_rate = std::clamp(genome.insert_evolution_rate + evolution_dist(rng), 0.0, 1.0);
    auto change_evolution_rate = std::clamp(genome.change_evolution_rate + evolution_dist(rng), 0.0, 1.0);
    auto delete_evolution_rate = std::clamp(genome.delete_evolution_rate + evolution_dist(rng), 0.0, 1.0);
    auto modify_a_evolution_rate = std::clamp(genome.modify_a_evolution_rate + evolution_dist(rng), 0.0, 1.0);
    auto modify_b_evolution_rate = std::clamp(genome.modify_b_evolution_rate + evolution_dist(rng), 0.0, 1.0);

    return {
            evolution_evolution_rate,
            insert_evolution_rate,
            change_evolution_rate,
            delete_evolution_rate,
            modify_a_evolution_rate,
            modify_b_evolution_rate,
            new_genes
    };
}

template<typename T>
class InclusiveIntegerRange : public ranges::v3::view_base
//class IntegerRange : public std::ranges::view_interface<IntegerRange<T>>
{
public:
    InclusiveIntegerRange() = default;

    explicit InclusiveIntegerRange(T start, T stop, T step) : start(start), stop(stop), step(step) {
        UUL_ASSERT(start < step ? step >= 1 : step <= -1);
    }

    explicit InclusiveIntegerRange(T start, T stop) : InclusiveIntegerRange(start, stop,
                                                                            start <= stop ? T{1} : T{-1}) {}

    explicit InclusiveIntegerRange(T stop) : InclusiveIntegerRange(T{0}, stop) {};

    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = T;
        using value_type = T;
        using pointer = T *;
        using reference = const T &;

        Iterator() = default;

        Iterator(T start, T stop, T step) : m_current(start), m_stop(stop), m_step(step) {}

        reference operator*() const { return m_current; }

        pointer operator->() { return &m_current; }

        Iterator &operator++() {
            m_current += m_step;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const Iterator &a, const Iterator &b) {
            return a.m_current == b.m_current;
        };

        friend bool operator!=(const Iterator &a, const Iterator &b) {
            return a.m_current != b.m_current;
        };

    private:
        T m_current;
        T m_stop;
        T m_step;
    };

    [[nodiscard]]
    Iterator begin() {
        return Iterator(start, stop, step);
    }

    [[nodiscard]]
    Iterator end() const {
        auto n = uul::integer_ceil((stop - start), step);
        auto new_stop = start + n * step;
        //should only be overshooting stop or equal to stop.
        if (new_stop == stop) {
            new_stop = start + (n + 1) * step;
        }
        return Iterator(new_stop, stop, step);
    }

private:
    T start;
    T stop;
    T step;
};



std::uint32_t le::create(le::Grid &grid, const le::Genome &genome, std::uint32_t origin_x, std::uint32_t origin_y) {
    std::int64_t min_value_x = std::numeric_limits<std::int64_t>::max();
    std::int64_t max_value_x = 0;
    std::int64_t min_value_y = std::numeric_limits<std::int64_t>::max();
    std::int64_t max_value_y = 0;
    std::int64_t x_pos = 64 / 2;
    std::int64_t y_pos = 64 / 2;
    constexpr auto null_genome_index = 0xFFFFFFFF;
    std::array<std::uint32_t, 64 * 64> genome_index_map = {null_genome_index};
    uul::Index2D<> index2D(64, 64);

    std::uint32_t body_segment_count = 0;

    //TODO create a stack and move backwards? commands rotate -128 -> 127
    //
    for (auto [idx, gene]: ranges::view::enumerate(genome.genes)) {
        switch (gene.genome_command) {
            case GenomeCommand::MoveUp: {
                auto index = index2D.to_linear_check(x_pos, y_pos - 1);
                if (index.has_value()) {
                    if (genome_index_map[index.value()] != null_genome_index) {
                        y_pos -= 1;
                    }
                }
                break;
            }
            case GenomeCommand::MoveDown: {
                auto index = index2D.to_linear_check(x_pos, y_pos + 1);
                if (index.has_value()) {
                    if (genome_index_map[index.value()] != null_genome_index) {
                        y_pos += 1;
                    }
                }
                break;
            }
            case GenomeCommand::MoveLeft: {
                auto index = index2D.to_linear_check(x_pos - 1, y_pos);
                if (index.has_value()) {
                    if (genome_index_map[index.value()] != null_genome_index) {
                        x_pos -= 1;
                    }
                }
                break;
            }
            case GenomeCommand::MoveRight: {
                auto index = index2D.to_linear_check(x_pos + 1, y_pos);
                if (index.has_value()) {
                    if (genome_index_map[index.value()] != null_genome_index) {
                        x_pos += 1;
                    }
                }
                break;
            }
            case GenomeCommand::AddUp: {
                auto index = index2D.to_linear_check(x_pos, y_pos - 1);
                if (index.has_value()) {
                    if (genome_index_map[index.value()] == null_genome_index) {
                        genome_index_map[index.value()] = idx;
                        body_segment_count += 1;
                    }
                    y_pos -= 1;
                }
                break;
            }
            case GenomeCommand::AddDown: {
                auto index = index2D.to_linear_check(x_pos, y_pos + 1);
                if (index.has_value()) {
                    if (genome_index_map[index.value()] == null_genome_index) {
                        genome_index_map[index.value()] = idx;
                        body_segment_count += 1;
                    }
                    y_pos += 1;
                }
                break;
            }
            case GenomeCommand::AddLeft: {
                auto index = index2D.to_linear_check(x_pos - 1, y_pos);
                if (index.has_value()) {
                    if (genome_index_map[index.value()] == null_genome_index) {
                        genome_index_map[index.value()] = idx;
                        body_segment_count += 1;
                    }
                    x_pos -= 1;
                }
                break;
            }
            case GenomeCommand::AddRight: {
                auto index = index2D.to_linear_check(x_pos + 1, y_pos);
                if (index.has_value()) {
                    if (genome_index_map[index.value()] == null_genome_index) {
                        genome_index_map[index.value()] = idx;
                        body_segment_count += 1;
                    }
                    x_pos += 1;
                }
                break;
            }
        }
        min_value_x = std::min(min_value_x, x_pos);
        max_value_x = std::max(max_value_x, x_pos);
        min_value_y = std::min(min_value_y, y_pos);
        max_value_y = std::max(max_value_y, y_pos);
    }
//        std::int64_t min_value_x = std::numeric_limits<std::int64_t>::max();
//        std::int64_t max_value_x = 0;
//        std::int64_t min_value_y = std::numeric_limits<std::int64_t>::max();
//        std::int64_t max_value_y = 0;
    std::uint32_t width = max_value_x - min_value_x;
    std::uint32_t height = max_value_y - min_value_y;

    pcg32 rng(pcg_extras::seed_seq_from<std::random_device>{});
    std::uniform_int_distribution<std::uint32_t> orientation_dist(0u, 3u);
    auto orientation = static_cast<le::Orientation>(orientation_dist(rng));

    auto cell_id = grid.organism_count();

    std::int64_t local_x_start = min_value_x;
    std::int64_t local_x_end = max_value_y + 1;

    std::int64_t local_y_start = min_value_y;
    std::int64_t local_y_end = max_value_y + 1;

    std::int64_t created_cell_count = 0;
    for (auto [local_y_idx, local_y]: ranges::views::enumerate(ranges::views::ints(local_x_start, local_x_end))) {
        for (auto [local_x_idx, local_x]: ranges::views::enumerate(ranges::views::ints(local_y_start, local_y_end))) {
            std::int64_t grid_x;
            std::int64_t grid_y;
            auto linear_index = index2D.to_linear(local_x, local_y);
            auto genome_index = genome_index_map[linear_index];
            auto gene = genome.genes[genome_index];
            switch (orientation) {
                case Orientation::Up : {
                    grid_x = origin_x + static_cast<std::int64_t>(local_x_idx);
                    grid_y = origin_y + static_cast<std::int64_t>(local_y_idx);
                    break;
                }
                case Orientation::Down: {
                    grid_x = origin_x - static_cast<std::int64_t>(local_x_idx);
                    grid_y = origin_y - static_cast<std::int64_t>(local_y_idx);
                    break;
                }
                case Orientation::Left: {
                    grid_x = origin_x - static_cast<std::int64_t>(local_x_idx);
                    grid_y = origin_y + static_cast<std::int64_t>(local_y_idx);
                    std::swap(grid_x, grid_y);
                    break;
                }
                case Orientation::Right: {
                    grid_x = origin_x + static_cast<std::int64_t>(local_x_idx);
                    grid_y = origin_y - static_cast<std::int64_t>(local_y_idx);
                    std::swap(grid_x, grid_y);
                    break;
                }
            }
            if (grid.cell_id_at(grid_x, grid_y) != consts::null_cell_id
                || grid_x < 0 || grid_x >= grid.width()
                || grid_y < 0 || grid_y >= grid.height()) {

                break;
            }
            created_cell_count += 1;
            grid.cell_id_at(grid_x, grid_y) = cell_id;
            grid.cell_type_at(grid_x, grid_y) = gene.cell_type;
            grid.cell_argument_a_at(grid_x, grid_y) = gene.cell_type_argument_a;
            grid.cell_argument_b_at(grid_x, grid_y) = gene.cell_type_argument_b;
            if (gene.cell_type == CellType::Scaffolding) {
                grid.cell_health_at(grid_x, grid_y) = consts::scaffolding_health;
            } else {
                grid.cell_health_at(grid_x, grid_y) = consts::cell_health_default;
            }
        }
    }
    if (created_cell_count > 0) {
        Phenotype phenotype{origin_x, origin_y, width, height, orientation, 0, 0, 0,
                            static_cast<std::uint32_t>(created_cell_count * consts::cell_initial_storage),
                            static_cast<std::uint32_t>(genome.genes.size()), 100};

        grid.add_organism(phenotype, genome);
    }

    return created_cell_count;

    //need to generate vector with all values for genome inside smaller than max?
    //or use as new layers on map? then just read from some origin point?
}

void cardinal_index_apply(std::int64_t x, std::int64_t y,
                          std::uint32_t width, std::uint32_t height,
                          const std::function<void(std::uint32_t x, std::uint32_t y)> &function) {
    if (x > 0) {
        function(x - 1, y);
    }
    if (x < (width - 1)) {
        function(x + 1, y);
    }
    if (y > 0) {
        function(x, y - 1);
    }
    if (height < (height - 1)) {
        function(x, y + 1);
    }
}

void cardinal_index_apply_with_enum(std::int64_t x, std::int64_t y,
                                    std::uint32_t width, std::uint32_t height,
                                    const std::function<void(std::uint32_t x, std::uint32_t y,
                                                             le::Orientation orientation)> &function) {
    if (x > 0) {
        function(x - 1, y, le::Orientation::Left);
    }
    if (x < (width - 1)) {
        function(x + 1, y, le::Orientation::Right);
    }
    if (y > 0) {
        function(x, y - 1, le::Orientation::Up);
    }
    if (height < (height - 1)) {
        function(x, y + 1, le::Orientation::Down);
    }
}

void cardinal_center_check_all_apply(std::int64_t x, std::int64_t y,
                                     uul::dim2d<std::uint32_t> dim,
                                     const std::function<void(std::uint32_t x, std::uint32_t y)> &function) {
    if (dim.is_within(glm::i64vec2{x, y})) {
        function(x, y);
    }
    if (dim.is_within(glm::i64vec2{x - 1, y})) {
        function(x - 1, y);
    }
    if (dim.is_within(glm::i64vec2{x + 1, y})) {
        function(x + 1, y);
    }
    if (dim.is_within(glm::i64vec2{x, y - 1})) {
        function(x, y - 1);
    }
    if (dim.is_within(glm::i64vec2{x, y + 1})) {
        function(x, y + 1);
    }
}


//TODO stream compact multiple at a time?
void le::process(Grid& grid_old, Grid& grid_new) {
    //set the new one to the old one so w
    grid_new = grid_old;
    for (auto index_y: ranges::views::ints(0u, grid_old.height())) {
        for (auto index_x: ranges::views::ints(0u, grid_old.width())) {
            auto meat_at = grid_old.meat_at(index_x, index_y);
            auto new_meat_at = uul::clamp_subtraction(meat_at, consts::meat_decay_rate);
            auto decayed = meat_at - new_meat_at;
            grid_new.meat_at(index_x, index_y) = new_meat_at;
            grid_new.detritus_at(index_x, index_y) = grid_old.detritus_at(index_x, index_y) + decayed;
        }
    }
    auto dim = uul::dim2d{grid_old.width(), grid_old.height()};
    for (auto index_y: ranges::views::ints(0u, grid_old.height())) {
        for (auto index_x: ranges::views::ints(0u, grid_old.width())) {
            if (grid_old.cell_type_at(index_x, index_y) == CellType::Producer) {
                auto current_cell_id = grid_old.cell_id_at(index_x, index_y);
                grid_new.get_phenotype(current_cell_id).energy_accumulation += consts::producer_rate;
//                grid.cell_output_a_grid(index_x, index_y) += consts::producer_rate;
            }
        }
    }
    for (auto index_y: ranges::views::ints(0u, grid_old.height())) {
        for (auto index_x: ranges::views::ints(0u, grid_old.width())) {
            if (grid_old.cell_type_at(index_x, index_y) == CellType::CellConsumer) {
                auto current_cell_id = grid_old.cell_id_at(index_x, index_y);
                auto energy_at = grid_old.meat_at(index_x, index_y);
                auto energy_consumed = energy_at < consts::meat_eat_rate ? energy_at : consts::meat_eat_rate;
                grid_new.meat_at(index_x, index_y) = energy_at - energy_consumed;
//                grid.cell_output_a_grid(index_x, index_y) += energy_consumed;
                grid_new.get_phenotype(current_cell_id).energy_accumulation += energy_consumed;
            }
        }
    }
    for (auto index_y: ranges::views::ints(0u, grid_old.height())) {
        for (auto index_x: ranges::views::ints(0u, grid_old.width())) {
            if (grid_old.cell_type_at(index_x, index_y) == CellType::DetritusConsumer) {
                auto current_cell_id = grid_old.cell_id_at(index_x, index_y);
                auto energy_at = grid_old.detritus_at(index_x, index_y);
                auto energy_consumed = energy_at < consts::detritus_eat_rate ? energy_at : consts::detritus_eat_rate;
                grid_new.detritus_at(index_x, index_y) = energy_at - energy_consumed;
//                grid.cell_output_a_grid(index_x, index_y) += energy_consumed;
                grid_new.get_phenotype(current_cell_id).energy_accumulation += energy_consumed;
            }
        }
    }
//    for(auto index_y : ranges::views::ints(0u,grid.height())){
//        for(auto index_x : ranges::views::ints(0u,grid.width())){
//            if(grid.cell_type_at(index_x, index_y) == CellType::Storage){
//                auto current_cell_id = grid.cell_id_at(index_x, index_y);
//                cardinal_index_apply(index_x, index_y, grid.width(), grid.height(),
//                 [&](std::uint32_t x, std::uint32_t y){
//                     auto adjacent_cell_id = grid.cell_id_at(x,y);
//                     if( adjacent_cell_id == current_cell_id){
//                         auto adjacent_cell_type = grid.cell_type_at(x,y);
//                         if(adjacent_cell_type == CellType::CellConsumer ||
//                            adjacent_cell_type == CellType::DetritusConsumer ||
//                            adjacent_cell_type == CellType::Producer){
//                             auto health_at = grid.cell_health_at(x, y);
//                             health_at = health_at < consts::detritus_eat_rate ? 0 : health_at - consts::detritus_eat_rate;
//                             grid.cell_health_at(x, y) = health_at;
//                         }
//                     }
//                 });
//            }
//        }
//    }

    for (auto index_y: ranges::views::ints(0u, grid_old.height())) {
        for (auto index_x: ranges::views::ints(0u, grid_old.width())) {
            if (grid_old.cell_type_at(index_x, index_y) == CellType::Seer) {
                auto current_cell_id = grid_old.cell_id_at(index_x, index_y);
                auto offset_x = std::bit_cast<std::int8_t>(grid_old.cell_argument_a_at(index_x, index_y));
                auto offset_y = std::bit_cast<std::int8_t>(grid_old.cell_argument_b_at(index_x, index_y));
                std::array<std::uint8_t, 5> max_seen_array = {};
                auto array_index = 0;
                //spit out what you see the max of?
                cardinal_center_check_all_apply(
                        index_x + offset_x, index_y + offset_y, dim,
                        [&](std::uint32_t x, std::uint32_t y) {
                            auto adjacent_cell_id = grid_old.cell_id_at(x, y);
                            if (adjacent_cell_id != current_cell_id) {
                                if (adjacent_cell_id == consts::null_cell_id) {
                                    auto meat_at = grid_old.meat_at(index_x, index_y);
                                    auto detritus_at = grid_old.detritus_at(index_x, index_y);
                                    if (meat_at > detritus_at) {
                                        auto value = static_cast<std::uint8_t>(CellType::End);
                                        max_seen_array[array_index] = value;
                                    } else if (detritus_at > 0) {
                                        auto value = static_cast<std::uint8_t>(CellType::Detritus);
                                        max_seen_array[array_index] = value;
                                    } else {
                                        max_seen_array[array_index] = 0;
                                    }
                                } else {
                                    auto cell_type = grid_old.cell_type_at(x, y);
                                    max_seen_array[array_index] = static_cast<std::uint8_t>(cell_type);
                                }
                            }
                            array_index += 1;
                        });

                //using a sorting network to quickly sort values.
                uul::StaticSort<5>()(max_seen_array);
                std::uint8_t max_run_value = 0;
                std::uint32_t max_run_size = 0;
                std::uint8_t current_run_value = max_seen_array[0];
                std::uint32_t current_run_size = 1;
                for (auto value: max_seen_array | ranges::views::drop(1)) {
                    if (value == current_run_value) {
                        current_run_size += 1;
                    } else {
                        if (current_run_size > max_run_size) {
                            max_run_value = current_run_value;
                            max_run_size = current_run_size;
                        }
                        current_run_value = value;
                        current_run_size = 1;
                    }
                }
                grid_new.cell_output_a_grid(index_x, index_y) = max_run_value;
            }
        }
    }

    for (std::size_t sub_index = 0; sub_index < consts::sub_index_iterations; ++sub_index) {
        for (auto index_y: ranges::views::ints(0u, grid_old.height())) {
            for (auto index_x: ranges::views::ints(0u, grid_old.width())) {
                if (grid_old.cell_type_at(index_x, index_y) == CellType::Neuron) {
                    auto current_cell_id = grid_old.cell_id_at(index_x, index_y);
                    auto weight = std::bit_cast<std::int8_t>(grid_old.cell_argument_a_at(index_x, index_y));
                    auto input_orientation = static_cast<uul::EnumFlags<InputOrientation>>(
                            grid_old.cell_argument_b_at(index_x, index_y) & 0b00001111
                    );
                    auto neuron_type = static_cast<NeuronMode>(
                            (grid_old.cell_argument_b_at(index_x, index_y) >> 4) % 3
                    );
                    double summation = 0;
                    cardinal_index_apply_with_enum(
                            index_x, index_y, grid_old.width(), grid_old.height(),
                            [&](std::uint32_t x, std::uint32_t y, Orientation orientation) {
                                switch (orientation) {
                                    case Orientation::Up:
                                        if (!input_orientation[InputOrientation::FromUp]) {
                                            return;
                                        }
                                        break;
                                    case Orientation::Down:
                                        if (!input_orientation[InputOrientation::FromDown]) {
                                            return;
                                        }
                                        break;
                                    case Orientation::Left:
                                        if (!input_orientation[InputOrientation::FromLeft]) {
                                            return;
                                        }
                                        break;
                                    case Orientation::Right:
                                        if (!input_orientation[InputOrientation::FromRight]) {
                                            return;
                                        }
                                        break;
                                }
                                auto adjacent_cell_id = grid_old.cell_id_at(x, y);
                                if (adjacent_cell_id == current_cell_id) {
                                    auto adjacent_cell_type = grid_old.cell_type_at(x, y);
                                    if (adjacent_cell_type == CellType::Neuron ||
                                        adjacent_cell_type == CellType::Seer) {
                                        if (neuron_type == NeuronMode::Select) {
                                            //use the new one because it gets updated
                                            if (static_cast<std::uint8_t>(grid_new.cell_output_a_grid(x, y)) ==
                                                (static_cast<std::uint8_t>(weight) % 16)) {
                                                summation += 1;
                                            }
                                        } else {
                                            summation += static_cast<double>(std::bit_cast<std::int8_t>(
                                                    grid_new.cell_output_a_grid(x, y)));
                                        }
                                    }
                                }
                            });

                    if (neuron_type == NeuronMode::Select) {
                        grid_new.cell_output_a_grid(index_x, index_y) = std::bit_cast<std::uint8_t>(
                                static_cast<std::int8_t>(summation));
                    } else if (neuron_type == NeuronMode::Multiply) {
                        auto true_weight = std::clamp((weight / 127.0), -1.0, 1.0) * 2.0;
                        grid_new.cell_output_a_grid(index_x, index_y) = std::bit_cast<std::uint8_t>(
                                static_cast<std::int8_t>((summation * true_weight / 2.0) * 127.0));
                    } else if (neuron_type == NeuronMode::Threshold) {
                        grid_new.cell_output_a_grid(index_x, index_y) = summation >= weight ? 1 : 0;
                    }
                }
            }
        }
    }

    // mover,

    // rotator , same loop? since so similar?
    // check at end?


    for (auto index_y: ranges::views::ints(0u, grid_old.height())) {
        for (auto index_x: ranges::views::ints(0u, grid_old.width())) {
            if (grid_old.cell_type_at(index_x, index_y) == CellType::Neuron) {
                auto current_cell_id = grid_old.cell_id_at(index_x, index_y);
                auto weight = std::bit_cast<std::int8_t>(grid_old.cell_argument_a_at(index_x, index_y));
                auto input_orientation = static_cast<uul::EnumFlags<InputOrientation>>(
                        grid_old.cell_argument_b_at(index_x, index_y) & 0b00001111
                );
                auto output_orientation = static_cast<Orientation>(
                        (grid_old.cell_argument_b_at(index_x, index_y) >> 4) % 4
                );
                double summation = 0;
                cardinal_index_apply_with_enum(
                        index_x, index_y, grid_old.width(), grid_old.height(),
                        [&](std::uint32_t x, std::uint32_t y, Orientation orientation) {
                            switch (orientation) {
                                case Orientation::Up:
                                    if (!input_orientation[InputOrientation::FromUp]) {
                                        return;
                                    }
                                    break;
                                case Orientation::Down:
                                    if (!input_orientation[InputOrientation::FromDown]) {
                                        return;
                                    }
                                    break;
                                case Orientation::Left:
                                    if (!input_orientation[InputOrientation::FromLeft]) {
                                        return;
                                    }
                                    break;
                                case Orientation::Right:
                                    if (!input_orientation[InputOrientation::FromRight]) {
                                        return;
                                    }
                                    break;
                            }
                            auto adjacent_cell_id = grid_old.cell_id_at(x, y);
                            if (adjacent_cell_id == current_cell_id) {
                                auto adjacent_cell_type = grid_old.cell_type_at(x, y);
                                if (adjacent_cell_type == CellType::Neuron ||
                                    adjacent_cell_type == CellType::Seer) {
                                    summation += static_cast<double>(std::bit_cast<std::int8_t>(
                                            grid_new.cell_output_a_grid(x, y)));
                                }
                            }
                        });

                float accumulation_coefficient = 0.0;
                if (static_cast<std::int8_t>(summation) > 0) {
                    accumulation_coefficient = 1.0;

                } else if (static_cast<std::int8_t>(summation) < 0) {
                    accumulation_coefficient = -1.0;
                }

                switch (output_orientation) {

                    case Orientation::Up:
                        grid_new.get_phenotype(current_cell_id).move_vertical_accumulation +=
                                consts::move_accumulation_default * accumulation_coefficient;
                        break;
                    case Orientation::Down:
                        grid_new.get_phenotype(current_cell_id).move_vertical_accumulation +=
                                consts::move_accumulation_default * -accumulation_coefficient;
                        break;
                    case Orientation::Left:
                        grid_new.get_phenotype(current_cell_id).move_horizontal_accumulation +=
                                consts::move_accumulation_default * -accumulation_coefficient;
                        break;
                    case Orientation::Right:
                        grid_new.get_phenotype(current_cell_id).move_horizontal_accumulation +=
                                consts::move_accumulation_default * accumulation_coefficient;
                        break;
                }
            }
        }
    }




    for (auto index_y: ranges::views::ints(0u, grid_old.height())) {
        for (auto index_x: ranges::views::ints(0u, grid_old.width())) {
            if (grid_old.cell_type_at(index_x, index_y) == CellType::Neuron) {
                auto current_cell_id = grid_old.cell_id_at(index_x, index_y);
                auto weight = std::bit_cast<std::int8_t>(grid_old.cell_argument_a_at(index_x, index_y));
                auto input_orientation = static_cast<uul::EnumFlags<InputOrientation>>(
                        grid_old.cell_argument_b_at(index_x, index_y) & 0b00001111
                );
                auto output_orientation = static_cast<RotateOrientation>(
                        (grid_old.cell_argument_b_at(index_x, index_y) >> 4) % 2
                );
                double summation = 0;
                cardinal_index_apply_with_enum(
                        index_x, index_y, grid_old.width(), grid_old.height(),
                        [&](std::uint32_t x, std::uint32_t y, Orientation orientation) {
                            switch (orientation) {
                                case Orientation::Up:
                                    if (!input_orientation[InputOrientation::FromUp]) {
                                        return;
                                    }
                                    break;
                                case Orientation::Down:
                                    if (!input_orientation[InputOrientation::FromDown]) {
                                        return;
                                    }
                                    break;
                                case Orientation::Left:
                                    if (!input_orientation[InputOrientation::FromLeft]) {
                                        return;
                                    }
                                    break;
                                case Orientation::Right:
                                    if (!input_orientation[InputOrientation::FromRight]) {
                                        return;
                                    }
                                    break;
                            }
                            auto adjacent_cell_id = grid_old.cell_id_at(x, y);
                            if (adjacent_cell_id == current_cell_id) {
                                auto adjacent_cell_type = grid_old.cell_type_at(x, y);
                                if (adjacent_cell_type == CellType::Neuron ||
                                    adjacent_cell_type == CellType::Seer) {
                                    summation += static_cast<double>(std::bit_cast<std::int8_t>(
                                            grid_old.cell_output_a_grid(x, y)));
                                }
                            }
                        });

                float accumulation_coefficient = 0.0;
                if (static_cast<std::int8_t>(summation) > 0) {
                    accumulation_coefficient = 1.0;

                } else if (static_cast<std::int8_t>(summation) < 0) {
                    accumulation_coefficient = -1.0;
                }
                if (output_orientation == RotateOrientation::left) {
                    grid_new.get_phenotype(current_cell_id).rotate_accumulation +=
                            consts::move_accumulation_default * accumulation_coefficient;
                } else if (output_orientation == RotateOrientation::right) {
                    grid_new.get_phenotype(current_cell_id).rotate_accumulation +=
                            consts::move_accumulation_default * -accumulation_coefficient;
                }
            }
        }
    }



    for (auto index_y: ranges::views::ints(0u, grid_old.height())) {
        for (auto index_x: ranges::views::ints(0u, grid_old.width())) {
            if (grid_old.cell_type_at(index_x, index_y) == CellType::Attacker) {
                auto current_cell_id = grid_old.cell_id_at(index_x, index_y);


                cardinal_index_apply(index_x, index_y, grid_old.width(), grid_old.height(),
                                     [&](std::uint32_t x, std::uint32_t y) {
                                         auto adjacent_cell_id = grid_old.cell_id_at(x, y);
                                         if (adjacent_cell_id != consts::null_cell_id &&
                                             adjacent_cell_id != current_cell_id) {
                                             auto health_at = grid_old.cell_health_at(x, y);
                                             health_at = health_at < consts::attack_rate ? 0 : health_at -
                                                                                               consts::attack_rate;
                                             grid_new.cell_health_at(x, y) = health_at;
                                         }
                                     });
            }
        }
    }

    for (auto index_y: ranges::views::ints(0u, grid_new.height())) {
        for (auto index_x: ranges::views::ints(0u, grid_new.width())) {
            if (grid_new.cell_type_at(index_x, index_y) == CellType::Attacker) {
                auto current_cell_id = grid_new.cell_id_at(index_x, index_y);
                auto health_at = grid_new.cell_health_at(index_x, index_y);
                if (health_at == 0) {
                    grid_new.kill_cell_at(index_x, index_y);
                }
            }
        }
    }

    auto organism_count = grid_new.organism_count();
    for (auto organism_index = 0; organism_index < organism_count; ++organism_index) {
        auto &phenotype = grid_new.get_phenotype(organism_index);
        if (phenotype.body_segment_count != 0) {
            UUL_ASSERT(phenotype.life_ticks > 0);
            phenotype.life_ticks -= 1;
            if(phenotype.life_ticks > 0) {
                auto accumulated_rotate_threshold = std::ceil(
                        phenotype.body_segment_count * consts::rotate_accumulation_coeff);
                auto accumulated_move_threshold = std::ceil(
                        phenotype.body_segment_count * consts::move_accumulation_coeff);
                if (std::abs(phenotype.rotate_accumulation) > accumulated_move_threshold) {
                    auto rotate_left = !std::signbit(phenotype.rotate_accumulation);
                    phenotype.rotate_accumulation = uul::sign(phenotype.rotate_accumulation) *
                                                    (std::abs(phenotype.rotate_accumulation) -
                                                            accumulated_rotate_threshold);
                    //rotate...
                    phenotype.energy_accumulation = uul::clamp_subtraction(phenotype.energy_accumulation,
                                                                           accumulated_rotate_threshold);
                    if (phenotype.energy_accumulation > 0) {
                        if(rotate_left){
                            bool vertical_even = phenotype.width % 2 == 0;
                            bool horizontal_even = phenotype.height % 2 == 0;
                            auto centroid = phenotype.centroid();
                            std::uint32_t move_fails = 0;
                            grid_old.for_every_pixel_in_organism(organism_index, [&](std::uint32_t x, std::uint32_t y){
                                glm::i64vec2 pos(static_cast<std::int64_t>(x),static_cast<std::int64_t>(y));
                                auto offset = pos - static_cast<glm::i64vec2>(centroid);
                                if(vertical_even && offset.x >= 0){
                                    offset.x += 1;
                                }
                                if(horizontal_even && offset.y >= 0){
                                    offset.y += 1;
                                }
                                offset.xy() = offset.yx();
                                if(rotate_left){
                                    offset.y *= -1;
                                }else{
                                    offset.x *= -1;
                                }
                                if(vertical_even && offset.x > 0){
                                    offset.x -= 1;
                                }
                                if(horizontal_even && offset.y > 0){
                                    offset.y -= 1;
                                }
                                auto end_point = static_cast<glm::i64vec2>(centroid) + offset;
                                //then check if the cell at the given point is our own? or another?
                                //TODO we need a seperate grid to make sure we don't lose data by accidentally over-writing ourselves...

                                if(grid_new.dim().is_within(end_point)){
                                    if(grid_new.cell_id_at(end_point.x, end_point.y) == consts::null_cell_id
                                    || grid_new.cell_id_at(end_point.x, end_point.y) == organism_index){

                                    }
                                }
                            });
                        }
                    }
                }
                if (std::abs(phenotype.move_vertical_accumulation) > accumulated_move_threshold) {
                    phenotype.move_vertical_accumulation = uul::sign(phenotype.move_vertical_accumulation) *
                                                           (std::abs(phenotype.move_vertical_accumulation) -
                                                            accumulated_move_threshold);
                    //rotate...
                    phenotype.energy_accumulation = uul::clamp_subtraction(phenotype.energy_accumulation,
                                                                           accumulated_move_threshold);
                    if (phenotype.energy_accumulation > 0) {
                        //do a move!
                    }
                }
                if (std::abs(phenotype.move_horizontal_accumulation) > accumulated_move_threshold) {
                    phenotype.move_horizontal_accumulation = uul::sign(phenotype.move_horizontal_accumulation) *
                                                             (std::abs(phenotype.move_horizontal_accumulation) -
                                                              accumulated_move_threshold);
                    //rotate...
                    phenotype.energy_accumulation = uul::clamp_subtraction(phenotype.energy_accumulation,
                                                                           accumulated_move_threshold);
                    if (phenotype.energy_accumulation > 0) {
                        //do a move!
                    }
                }
            }
            if(phenotype.life_ticks <= 0 || phenotype.energy_accumulation <= 0){
                grid.kill_organism(organism_index);
                UUL_ASSERT(phenotype.body_segment_count == 0);
            }
        }
        //remove organisms if no body,
        //remove organism if no energy?
        //remove if life ticks 0?  need to do this as the last thing?s
    }

    for (auto index_y: ranges::views::ints(0u, grid.height())) {
        for (auto index_x: ranges::views::ints(0u, grid.width())) {

            if (grid.cell_type_at(index_x, index_y) == CellType::Attacker) {
                auto current_cell_id = grid.cell_id_at(index_x, index_y);
                auto health_at = grid.cell_health_at(index_x, index_y);
                if (health_at == 0) {
                    grid.kill_cell_at(index_x, index_y);
                }
            }
        }
    }


}

le::Orientation le::rotate_left(le::Orientation orientation) {
    return static_cast<Orientation>((static_cast<std::uint8_t>(orientation) + 1) % 4);
}

le::Orientation le::rotate_right(le::Orientation orientation) {
    auto value = static_cast<std::uint8_t>(orientation);
    return static_cast<Orientation>((value > 0 ? value - 1 : 3));
}

bool le::Phenotype::is_dead() const {
    return body_segment_count == 0 || energy_accumulation == 0 || life_ticks == 0;
}

glm::uvec2 le::Phenotype::centroid() const {
    std::uint32_t centroid_x = origin_x + ((origin_x + width) / 2.0);
    std::uint32_t centroid_y = origin_y + ((origin_y + height) / 2.0);
    return {centroid_x, centroid_y};
}
