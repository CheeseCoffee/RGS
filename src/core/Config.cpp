#include "Config.h"

#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

Config* Config::_instance = nullptr;

void Config::init() {
    if (_normalizer == nullptr) {
        _normalizer.reset(new Normalizer());
    }

    double maxMass = 0.0, maxRadius = 0.0;
    for (const auto& gas : _gases) {
        maxMass = std::max(maxMass, gas.getMass());
        maxRadius = std::max(maxRadius, gas.getRadius());
    }
    double maxPressure = 0.0, maxTemperature = 0.0;
    for (auto& initialParam : _initialParameters) {
        for (auto gi = 0; gi < _gases.size(); gi++) {
            maxPressure = std::max(maxPressure, initialParam.getPressure(gi));
            maxTemperature = std::max(maxTemperature, initialParam.getTemperature(gi));
        }
    }
    _normalizer->init(maxMass, maxRadius, maxPressure, maxTemperature);

    for (auto& gas : _gases) {
        gas.setMass(_normalizer->normalize(gas.getMass(), Normalizer::Type::MASS));
        gas.setRadius(_normalizer->normalize(gas.getRadius(), Normalizer::Type::RADIUS));
    }
    for (auto& betaChain : _betaChains) {
        betaChain.setLambda1(_normalizer->normalize(betaChain.getLambda1(), Normalizer::Type::LAMBDA));
        betaChain.setLambda2(_normalizer->normalize(betaChain.getLambda2(), Normalizer::Type::LAMBDA));
    }
    for (auto& initialParam : _initialParameters) {
        for (auto gi = 0; gi < _gases.size(); gi++) {
            initialParam.setPressure(gi, _normalizer->normalize(initialParam.getPressure(gi), Normalizer::Type::PRESSURE));
            initialParam.setTemperature(gi, _normalizer->normalize(initialParam.getTemperature(gi), Normalizer::Type::TEMPERATURE));
        }
    }
    for (auto& boundaryParam : _boundaryParameters) {
        for (auto gi = 0; gi < _gases.size(); gi++) {
            boundaryParam.setPressure(gi, _normalizer->normalize(boundaryParam.getPressure(gi), Normalizer::Type::PRESSURE));
            boundaryParam.setTemperature(gi, _normalizer->normalize(boundaryParam.getTemperature(gi), Normalizer::Type::TEMPERATURE));

            Vector3d flow = boundaryParam.getFlow(gi);
            _normalizer->normalize(flow.x(), Normalizer::Type::FLOW);
            _normalizer->normalize(flow.y(), Normalizer::Type::FLOW);
            _normalizer->normalize(flow.z(), Normalizer::Type::FLOW);
            boundaryParam.setFlow(gi, flow);
        }
    }

    if (_impulseSphere == nullptr) {
        _impulseSphere.reset(new ImpulseSphere(4.8, 20));
    }
    _impulseSphere->init();
}

void Config::load(const std::string& filename) {
    boost::property_tree::ptree root;
    boost::property_tree::read_json(filename, root);

    _meshFilename = root.get<std::string>("mesh", "");
    _meshUnits = root.get<double>("mesh_units", 1.0);

    _outputFolder = root.get<std::string>("output_folder", "./");
    _maxIterations = root.get<unsigned int>("max_iterations", 0);
    _outEachIteration = root.get<unsigned int>("out_each_iteration", 1);
    _isUsingIntegral = root.get<bool>("use_integral", false);
    _isUsingBetaDecay = root.get<bool>("use_beta_decay", false);

    _gases.clear();
    auto gasesNode = root.get_child_optional("gases");
    if (gasesNode) {
        for (const boost::property_tree::ptree::value_type& gas : *gasesNode) {
            auto mass = gas.second.get<double>("mass") * 1.66e-27; // aem to kg
            auto radius = gas.second.get<double>("radius") * 1e-12; // rad
            _gases.emplace_back(mass, radius);
        }
    }

    _betaChains.clear();
    auto betaChainsNode = root.get_child_optional("beta_chains");
    if (betaChainsNode) {
        for (const boost::property_tree::ptree::value_type& betaChain : *betaChainsNode) {
            auto gi1 = betaChain.second.get<unsigned int>("gi1");
            auto gi2 = betaChain.second.get<unsigned int>("gi2");
            auto gi3 = betaChain.second.get<unsigned int>("gi3");
            auto lambda1 = betaChain.second.get<double>("lambda1");
            auto lambda2 = betaChain.second.get<double>("lambda2");
            _betaChains.emplace_back(gi1, gi2, gi3, lambda1, lambda2);
        }
    }

    _initialParameters.clear();
    auto initalNode = root.get_child_optional("initial");
    if (initalNode) {
        for (const boost::property_tree::ptree::value_type& param : *initalNode) {
            auto group = param.second.get<std::string>("group");

            std::vector<double> pressure;
            for (const boost::property_tree::ptree::value_type& value : param.second.get_child("pressure")) {
                pressure.emplace_back(value.second.get_value<double>());
            }
            pressure.resize(_gases.size(), 0.0);

            std::vector<double> temperature;
            for (const boost::property_tree::ptree::value_type& value : param.second.get_child("temperature")) {
                temperature.emplace_back(value.second.get_value<double>());
            }
            temperature.resize(_gases.size(), 0.0);

            _initialParameters.emplace_back(group, pressure, temperature);
        }
    }

    _boundaryParameters.clear();
    auto boundaryNode = root.get_child_optional("boundary");
    if (boundaryNode) {
        for (const boost::property_tree::ptree::value_type& param : *boundaryNode) {
            auto group = param.second.get<std::string>("group");

            std::vector<std::string> type;
            auto typeNode = param.second.get_child_optional("type");
            if (typeNode) {
                for (const boost::property_tree::ptree::value_type& value : *typeNode) {
                    type.emplace_back(value.second.get_value<std::string>());
                }
            }

            std::vector<double> pressure;
            auto pressureNode = param.second.get_child_optional("pressure");
            if (pressureNode) {
                for (const boost::property_tree::ptree::value_type& value : *pressureNode) {
                    pressure.emplace_back(value.second.get_value<double>());
                }
            }
            pressure.resize(_gases.size(), 0.0);

            std::vector<double> temperature;
            auto temperatureNode = param.second.get_child_optional("temperature");
            if (temperatureNode) {
                for (const boost::property_tree::ptree::value_type& value : *temperatureNode) {
                    temperature.emplace_back(value.second.get_value<double>());
                }
            }
            temperature.resize(_gases.size(), 0.0);

            std::vector<Vector3d> flow;
            auto flowNode = param.second.get_child_optional("flow");
            if (flowNode) {
                for (const boost::property_tree::ptree::value_type& value : *flowNode) {
                    double flowX = value.second.get<double>("x", 0);
                    double flowY = value.second.get<double>("y", 0);
                    double flowZ = value.second.get<double>("z", 0);
                    flow.emplace_back(flowX, flowY, flowZ);
                }
            }
            flow.resize(_gases.size(), Vector3d());

            _boundaryParameters.emplace_back(group, type, temperature, pressure, flow);
        }
    }
}

std::ostream& operator<<(std::ostream& os, const Config& config) {
    os << "MeshFilename = "     << config._meshFilename                        << std::endl
       << "OutputFolder = "     << config._outputFolder                        << std::endl
       << "MaxIteration = "     << config._maxIterations                       << std::endl
       << "OutEachIteration = " << config._outEachIteration                    << std::endl
       << "UseIntegral = "      << config._isUsingIntegral                     << std::endl
       << "UseBetaDecay = "     << config._isUsingBetaDecay                    << std::endl;

    os << "Gases = "            << Utils::toString(config._gases)              << std::endl;
    os << "BetaChains = "       << Utils::toString(config._betaChains)         << std::endl;
    os << "Initial = "          << Utils::toString(config._initialParameters)  << std::endl;
    os << "Boundary = "         << Utils::toString(config._boundaryParameters) << std::endl;

    os << "Normalizer = "       << *config._normalizer                         << std::endl
       << "ImpulseSphere = "          << *config._impulseSphere;
    return os;
}
