#include "solver.h"

#include "parallel.h"
#include "solver_info.h"
#include "options.h"
#include "grid.h"
#include "cell.h"

Solver::Solver()
: m_pParallel(new Parallel),
  m_pSolverInfo(new SolverInfo),
  m_pGrid(new Grid)
{
	// TODO Auto-generated constructor stub
}

Solver::~Solver() {
	// TODO Auto-generated destructor stub
}

void Solver::Init() {
	// We must OBEY order:
	// first info (impulse, etc)
	// second grid....
  m_pSolverInfo->setSolver(this);
  m_pSolverInfo->Init();

  // Init grid
  m_pGrid->SetParent(this);
  m_pGrid->Init();
}

void Solver::Run() {
	std::vector<std::shared_ptr<Cell>>& cellVector = m_pGrid->getCellVector();
	for(int iteration = 0;iteration<m_pSolverInfo->getMaxIteration();iteration++) {
		makeStep(sep::X);
		makeStep(sep::Y);
		makeStep(sep::Z);

		// here we can test data, if needed...
		for( auto& item : cellVector ) {
			item->testInnerValuesRange();
		}

		// Saving data

	}
}

void Solver::makeStep(sep::Axis axis) {
	std::vector<std::shared_ptr<Cell>>& cellVector = m_pGrid->getCellVector();
	// make half
	for( auto& item : cellVector ) {
		item->computeHalf(axis);
	}
	// make value
	for( auto& item : cellVector ) {
		item->computeValue(axis);
	}
	// make integral
	// TODO: add integral count...
	for( auto& item : cellVector ) {
		item->computeIntegral(axis);
	}
}

Config* Solver::GetActiveConfig() const {
  return m_pSolverInfo->getOptions()->GetActiveConfig();
}
