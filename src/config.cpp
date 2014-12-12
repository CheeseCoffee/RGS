#include "config.h"

using namespace std;

std::string Config::sName = "default";
Vector3i Config::vGridSize = Vector3i(10, 10, 1);
Vector3i Config::vOutputGridStart = Vector3i();
Vector3i Config::vOutputSize = Vector3i(10, 10, 1);
sep::GridGeometry Config::eGridGeometryType = sep::COMB_GRID_GEOMETRY;
double Config::dTimestep = 0.01;
int Config::iMaxIteration = 100;
bool Config::bUseIntegral = true;
std::shared_ptr<HTypeGridConfig> Config::pHTypeGridConfig = std::shared_ptr<HTypeGridConfig>(new HTypeGridConfig);
std::string Config::sOutputPrefix = "../";

void Config::PrintMe() {
  cout << "Config " << sName << endl;
  cout << "GridSize = " << vGridSize.x() <<
  "x" << vGridSize.y() <<
  "x" << vGridSize.z() << endl;
  cout << "OutputSize = " << vOutputSize.x() <<
  "x" << vOutputSize.y() <<
  "x" << vOutputSize.z() << endl;
}
