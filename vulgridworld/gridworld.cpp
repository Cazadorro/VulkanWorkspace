//
// Created by Bolt on 2/3/2023.
//

#include "gridworld.h"
#include "uul/assert.h"
#include "uul/aliasmethod.h"
#include <range/v3/view/enumerate.hpp>
#include <array>
#include <algorithm>
#include <vector>
#include <random>
#include <functional>



template<typename T, std::uint64_t width_val, std::uint64_t height_val>
class WorldLayer{
public:
    static constexpr auto width = width_val;
    static constexpr auto height = height_val;
    [[nodiscard]]
    constexpr std::uint64_t size() const{
        return width * height;
    }
    const T& operator()(std::uint64_t row, std::uint64_t col) const{
        return m_data[row * width + col];
    }
    T& operator()(std::uint64_t row, std::uint64_t col){
        return m_data[row * width + col];
    }
    const T& operator()(std::uint64_t idx) const{
        return m_data[idx];
    }
    T& operator()(std::uint64_t idx){
        return m_data[idx];
    }
    void fill(const T& value){
        std::fill(m_data.begin(), m_data.end(), value);
    }
    WorldLayer() = default;
    explicit WorldLayer(const T& value){
        std::fill(m_data.begin(), m_data.end(), value);
    }
private:
    std::array<T, width * height> m_data;
};


struct InputEdge{
    std::uint32_t source_id;
    float weight;
};
struct LayerNode{
    std::vector<InputEdge> input_edges;
    float bias;
};


struct OutputLayerMap{
    std::uint32_t do_nothing;
    std::uint32_t move_north;
    std::uint32_t move_south;
    std::uint32_t move_east;
    std::uint32_t move_west;
    std::uint32_t turn_north;
    std::uint32_t turn_south;
    std::uint32_t turn_east;
    std::uint32_t turn_west;
    std::uint32_t eat_north;
    std::uint32_t eat_south;
    std::uint32_t eat_east;
    std::uint32_t eat_west;
    std::uint32_t attack_north;
    std::uint32_t attack_south;
    std::uint32_t attack_east;
    std::uint32_t attack_west;
};

struct OutputLayerValues{
    std::uint32_t do_nothing;

    std::uint32_t move_north;
    std::uint32_t move_south;
    std::uint32_t move_east;
    std::uint32_t move_west;

    std::uint32_t turn_north;
    std::uint32_t turn_south;
    std::uint32_t turn_east;
    std::uint32_t turn_west;

    std::uint32_t eat_north;
    std::uint32_t eat_south;
    std::uint32_t eat_east;
    std::uint32_t eat_west;

    std::uint32_t attack_north;
    std::uint32_t attack_south;
    std::uint32_t attack_east;
    std::uint32_t attack_west;
};
//initialization
//representation
//parent selection
//crossover operators
//mutation operators
//survival selecction
//termination condition

std::random_device dev;
std::uint64_t seed = 3234;
std::minstd_rand lcg_rng(seed);
float ReLU(float x){
    return std::max(0.0f, x);
}

static constexpr std::uint32_t output_layer_count = 17;

struct EdgeGene{
    std::uint32_t innovation_id;
    std::uint32_t src;
    std::uint32_t dst;
    float weight;
    bool disable;
};
struct NeuronGenome{
    std::vector<EdgeGene> edges;
};
enum class MutationType{
    AddEdge = 0,
    AddNode = 1,
    ModifyEdge = 2,
//    RemoveEdge = 3
};
std::array probabilities = {0.1, 0.1, 0.8};
struct NeuronMutator{
    NeuronMutator() : alias_table(probabilities){
    }
    uul::AliasTable<> alias_table;
    std::vector<EdgeGene> AddEdge(const EdgeGene& edge, std::uint32_t current_max_nodes) const{
        //to prevent cycles, make sure that edge from A cannot connect to value <= A.

    }
    std::vector<EdgeGene> operator() () const{

    }
};

template<typename RngEngine_T>
NeuronGenome mutate(const NeuronGenome& genome, RngEngine_T& rng){
    NeuronGenome new_genome;
    for(const auto & edge : genome.edges){
        new_genome.edges.push_back()
    }
}
//add edge
//add node
//modify
//remove edge? or no?


struct Neurons{
    std::vector<std::vector<LayerNode>> layers;
    Neurons() = default;
    template<typename RngEngine_T>
    explicit Neurons(RngEngine_T& rng){
        std::uniform_real_distribution<float> dist(-2.0, 2.0);
        std::uniform_int_distribution<std::uint32_t> count_dist(0,3);
        std::uniform_int_distribution<std::uint32_t> gen_idx(0, 0xFFFFFFFF);
        auto layer_count = count_dist(rng);
        for(std::size_t i = 0; i < layer_count + 1; ++i){
            auto layer_size = i != layer_count ? count_dist(rng) : output_layer_count;
            LayerNode node;
            node.input_edges.reserve(layer_size);
            for(std::size_t layer_idx = 0; layer_idx < layer_size; ++layer_idx){
                InputEdge edge{gen_idx(rng), dist(rng)};
                node.input_edges.push_back(edge);
            }
            node.bias = dist(rng);
        }
    }

    static Neurons mutate(const Neurons& neurons){

    }
    static Neurons recombine(const Neurons& lhs, const Neurons& rhs){
        Neurons neurons;
        neurons.layers =
    }
    std::array<float, output_layer_count> inference(const std::vector<float>& inputs, const std::function<float(float)>& activation_function){
        auto layer_inputs = inputs;
        for(const auto& layer : layers){
            std::vector<float> layer_outputs(layer.size());
            for(const auto [i, node] : layer | ranges::view::enumerate){
                float sum = 0;
                for(const auto edge : node.input_edges){
                    sum += layer_inputs[edge.source_id % layer_inputs.size()] * edge.weight;
                }
                layer_outputs[i] = activation_function(sum + node.bias);
            }
            layer_inputs = layer_outputs;
        }
        UUL_ASSERT(layer_inputs.size() == output_layer_count, "Expected output layer to match number of outputs");
        std::array<float, output_layer_count> output_layer = {};
        std::copy(layer_inputs.begin(), layer_inputs.end(), output_layer.begin());
        return output_layer;
    }
};

//input genome
//output genome?
struct Genome{
    Neurons m_neural_genome;
    std::array<std::uint32_t, 16> m_codon_commands;
    std::array<float, 16> m_codon_values;
};
//value per phenotype, mixing?
//repeat N
//start/stop
//empty?
//when age X
//when energy >x

enum class PhenotypeControl : std::uint32_t{
    Repeat,
    StopNone,
    AgeThresholdGT,
    ArmorNorth,
    ArmorSouth,
    ArmorEast,
    ArmorWest,
    SizeVal,
    AttackBlunt,
    AttackSlash,
    AttackPierce,
    EnergyCapacity
};
//armor
//size
//mass
//speed
//sprint speed
//sprint capacity
//attacks
//energy
//color and/or pattern?
//hp/
//ears / sound
//nose / smell
//touch
//offspring count
//spikes

//run efficiency?  how should efficiency and tradeoffs be represented.
//swim/dig?




//move
//turn
//eat
//attack

struct Armor{
    float north;
    float south;
    float east;
    float west;
};

struct Health{
    float value;
};

struct Mass{
    float value;
};

struct Size{
    float value;
};

struct Attack{
    float blunt;
    float slash;
    float pierce;
};

struct Energy{
    float value;
};


struct Age{
    float value;
};

struct Animals{
    std::vector<Genome> genomes;
    std::vector<Armor> armor;
    std::vector<Health> health;
    std::vector<Mass> mass;
    std::vector<Size> size;
    std::vector<Attack> attack;
    std::vector<Energy> energy;
    std::vector<Neurons> neurons;
    std::vector<std::uint32_t> age;

};
void grid::test() {

    WorldLayer<std::uint64_t, 128u, 128u> id_layer(0xFFFFFFFFFFFFFFFF);
    WorldLayer<std::uint8_t, 128u, 128u> plant_layer(0xFF);



}
