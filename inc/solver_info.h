/*
 * solver_info.h
 *
 *  Created on: 04 апр. 2014 г.
 *      Author: kisame
 */

#ifndef SOLVER_INFO_H_
#define SOLVER_INFO_H_


#include <vector>
#include <memory>

class Options;
class Impulse;
class Gas;


class SolverInfo {
	typedef std::vector< std::shared_ptr<Gas> > GasVector;
private:
	std::shared_ptr<Impulse> m_pImpulse;
	std::shared_ptr<Options> m_pOptions;
	GasVector m_vGas;
public:
	SolverInfo();
	virtual ~SolverInfo();

	Options* getOptions() { return m_pOptions.get(); }
	Impulse* getImpulse() { return m_pImpulse.get(); }
	GasVector& getGasVector() { return m_vGas; }
};

#endif /* SOLVER_INFO_H_ */
