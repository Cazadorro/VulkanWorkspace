//
// Created by Bolt on 6/25/2023.
//

#ifndef VULKANWORKSPACE_LIFEENGINEGRID_H
#define VULKANWORKSPACE_LIFEENGINEGRID_H

#include <uul/indexing.h>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/zip.hpp>
#include <vector>
#include <cstdint>
#include <array>


namespace le {

    namespace consts{
        std::uint8_t cell_health_default = 10;
        std::uint64_t null_cell_id = 0xff'ff'ff'ff'ff'ff'ff'ff;
        std::uint8_t producer_rate = 1u;
        std::uint8_t meat_eat_rate = 10u;
        std::uint8_t attack_rate = 10u;
        std::uint8_t detritus_eat_rate = 10u;
        std::uint8_t meat_decay_rate = 1u;
        std::uint8_t cell_create_cost = 2u;
        std::uint8_t cell_initial_storage = 1u;
        std::uint8_t cell_death_cost = cell_create_cost - cell_initial_storage;
        std::uint8_t scaffolding_health = 100u;
        std::size_t sub_index_iterations = 16u;
        float rotate_accumulation_coeff = 0.25;
        float move_accumulation_coeff = 1.0;
        float move_accumulation_default = 1.0;
        float rotate_accumulation_default = 4.0;
        std::uint32_t energy_breed_muliplier = 4.0;
    }

    enum class CellType : std::uint8_t {
        Void = 0,             // Nothing
        Producer,             // generates energy
        //TODO even have this?  What is the point? Maybe max energy storage increased, and allow thigns that eat it to
        //TODO make it seem like fruit
        //Storage,              // consumes energy, stores it, provides it for the entire system
        Mover,                // consumes energy for every cell type attached, % moves per turn, randomly moves object without input.

        Rotator,              // consumes energy for every cell type attached, % rotate per turn, randomly rotates object with out input.

        // curve? energy / output, area under the curve must be the same? slope? figure out Y intercept, can make fast take less energy, but slow takes more?
        //

        Attacker,             // consumes energy, attack objects adjacent.
        Scaffolding,          // defends against attacks, connects adjacent nodes?
        CellConsumer,         // consumes adjacent foreign cells, turns them into energy
        DetritusConsumer,     // consumes expired foreign cell after math, turns them into energy

        //TODO healer based on *genome*, healer: takes a given genome coordinate and creates stem cells which start attempting to create things at the given
        //TODO location based on genome instruction, if unobstructed.  Stops when genome stops or when stops being obstructed? or maybe genome is given a "stop point"
        // u16 genome start, u16 genome end? for now u8 start and u8 end?
        Healer,               // repairs adjacent cell using energy
        Seer,                 // sees adjacent cell + seers in the same direction for further sight + 1 energy usage? (liner or quadratic? every eye attached increases sight further?)
                              // look at another eye = see farther).
                              // eyes would be default see in a 3x3 area around eye, any block puts +1 in the opposite direction of the eye where blocked.
                              //
        Neuron,               // given input signal, sends output (and?) -1, 0, 1
        End,
        Detritus,
    };

    //unorm neurons?
    std::uint8_t cell_type_end = static_cast<std::uint8_t>(CellType::End);


    enum class Orientation : std::uint8_t{
        Up,
        Left,
        Down,
        Right,
    };
    [[nodiscard]]
    Orientation rotate_left(Orientation orientation);
    [[nodiscard]]
    Orientation rotate_right(Orientation orientation);

    enum class RotateOrientation : std::uint8_t{
        left,
        right,
    };



    enum class InputOrientation : std::uint8_t{
        FromUp    = 0b0001,
        FromDown  = 0b0010,
        FromLeft  = 0b0100,
        FromRight = 0b1000,
    };

    enum class NeuronMode : std::uint8_t{
        Multiply,
        Select,
        Threshold,
    };



    enum class GenomeCommand : std::uint8_t{
        MoveUp,
        MoveDown,
        MoveLeft ,
        MoveRight ,

        AddUp,
        AddDown,
        AddLeft,
        AddRight,
    };
    std::uint8_t genome_command_end = static_cast<std::uint8_t>(GenomeCommand::AddRight) + 1;


    enum class GenomeCommand2 : std::uint8_t {
        Rotate,
        Place,
        BackTrack
    };



    struct alignas(4) Gene{
        CellType cell_type;
        GenomeCommand genome_command;
        std::uint8_t cell_type_argument_a;
        std::uint8_t cell_type_argument_b;
    };
    static_assert(sizeof(Gene) == 4);

    struct Genome{
        //how fast to evolve
        double evolution_evolution_rate = 1.0;
        double insert_evolution_rate = 1.0;
        double change_evolution_rate = 1.0;
        double delete_evolution_rate = 1.0;
        double modify_a_evolution_rate = 1.0;
        double modify_b_evolution_rate = 1.0;
        //todo lifespan
        //todo energy before breed
        std::vector<Gene> genes;
    };
    [[nodiscard]]
    Genome mutate(const Genome& genome);

    struct Phenotype{
        std::uint32_t origin_x;
        std::uint32_t origin_y;
        std::uint32_t width;
        std::uint32_t height;
        Orientation orientation;
        float rotate_accumulation = 0;
        float move_vertical_accumulation = 0;
        float move_horizontal_accumulation = 0;
        std::uint32_t energy_accumulation = 0;
        //TODO max energy?
        std::uint32_t body_segment_count = 0;
        std::uint32_t life_ticks;
        [[nodiscard]]
        bool is_dead() const;
        [[nodiscard]]
        glm::uvec2 centroid() const;
    };



    class Grid {
    public:
        explicit Grid(std::uint32_t width, std::uint32_t height);

        std::uint64_t add_organism(Phenotype phenotype, Genome genome);

        [[nodiscard]]
        std::uint64_t organism_count() const;

        [[nodiscard]]
        const Phenotype& get_phenotype(std::uint64_t cell_id) const;
        [[nodiscard]]
        Phenotype& get_phenotype(std::uint64_t cell_id);

        void for_every_pixel_in_organism(std::uint64_t organism_index, const std::function<void(std::uint32_t x, std::uint32_t y)>& function);

        void kill_organism(std::uint64_t organism_index);
        void kill_cell_at(std::uint32_t x, std::uint32_t y);
        void clear_cell_at(std::uint32_t x, std::uint32_t y);
        bool move_cell_to(glm::uvec2 current_pos, glm::uvec2 final_pos);
        [[nodiscard]]
        CellType cell_type_at(std::uint32_t x, std::uint32_t y) const;

        [[nodiscard]]
        std::uint8_t cell_argument_a_at(std::uint32_t x, std::uint32_t y) const;
        [[nodiscard]]
        std::uint8_t cell_argument_b_at(std::uint32_t x, std::uint32_t y) const;
        [[nodiscard]]
        std::uint8_t cell_output_a_grid(std::uint32_t x, std::uint32_t y) const;
        [[nodiscard]]
        std::uint8_t& cell_output_a_grid(std::uint32_t x, std::uint32_t y);
        [[nodiscard]]
        std::uint8_t cell_health_at(std::uint32_t x, std::uint32_t y) const;
        [[nodiscard]]
        std::uint64_t cell_id_at(std::uint32_t x, std::uint32_t y) const;


        [[nodiscard]]
        std::uint32_t meat_at(std::uint32_t x, std::uint32_t y) const;

        [[nodiscard]]
        std::uint32_t detritus_at(std::uint32_t x, std::uint32_t y) const;


        [[nodiscard]]
        CellType& cell_type_at(std::uint32_t x, std::uint32_t y);

        [[nodiscard]]
        std::uint8_t& cell_argument_a_at(std::uint32_t x, std::uint32_t y);
        [[nodiscard]]
        std::uint8_t& cell_argument_b_at(std::uint32_t x, std::uint32_t y);
        [[nodiscard]]
        std::uint8_t& cell_health_at(std::uint32_t x, std::uint32_t y);
        [[nodiscard]]
        std::uint64_t& cell_id_at(std::uint32_t x, std::uint32_t y);

        [[nodiscard]]
        std::uint32_t& meat_at(std::uint32_t x, std::uint32_t y);

        [[nodiscard]]
        std::uint32_t& detritus_at(std::uint32_t x, std::uint32_t y);

        [[nodiscard]]
        std::uint32_t width() const;

        [[nodiscard]]
        std::uint32_t height() const;

        [[nodiscard]]
        uul::dim2d<std::uint32_t> dim() const;

        [[nodiscard]]
        std::uint64_t size() const;

        [[nodiscard]]
        std::size_t linear_index(std::uint32_t x, std::uint32_t y) const;

        void fill_empty_organism_list();
    private:
        std::uint32_t m_width;
        std::uint32_t m_height;
        std::vector<CellType> m_cell_type_grid;
        std::vector<std::uint8_t> m_cell_argument_a_grid;
        std::vector<std::uint8_t> m_cell_argument_b_grid;
        std::vector<std::uint8_t> m_cell_output_a_grid;
        std::vector<std::uint8_t> m_cell_health_grid;
        std::vector<std::uint64_t> m_cell_id_grid;
        std::vector<std::uint32_t> m_meat_energy_grid;
        std::vector<std::uint32_t> m_detritus_energy_grid;
        std::vector<Phenotype> m_phenotypes;
        std::vector<Genome> m_genomes;
        std::vector<std::uint32_t> m_empty_organism_list;
    };

    // for every id, start from origin? where to start? based on how cell is created?
//returns cell cost
    std::uint32_t create(Grid& grid, const Genome& genome, std::uint32_t origin_x, std::uint32_t origin_y);

    void process(Grid& grid_old, Grid& grid_new);

}
#endif //VULKANWORKSPACE_LIFEENGINEGRID_H
