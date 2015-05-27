#ifndef GRID_CONSTRUCTOR_H_
#define GRID_CONSTRUCTOR_H_

#include "main.h"

// GridManager interface
class GridConstructor {
protected:
    void ConfigureStandartGrid();
    void ConfigureGPRT();
	void ConfigureGPRT2();
	void BoundaryConditionTest();
	void ConfigureGPRT4();
	void ConfigureGPRT5();
  
private:
    virtual void PushTemperature(double dT) = 0;
    virtual double PopTemperature() = 0;
    virtual void PushPressure(double pressure) = 0;
    virtual double PopPressure() = 0;
    virtual void PrintGrid() = 0;
    // Set box to normal
    // 0 and (size - 1) - fake cells; size = box_size + 2
    virtual void SetBox(Vector2d vP, Vector2d vSize,
          ConfigFunction config_func =
          [] (int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {}) = 0;
};

#endif // GRID_CONSTRUCTOR_H_