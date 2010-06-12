/*
 * RzMotionTimer.h
 *
 *  Created on: Jun 11, 2010
 *      Author: ross
 */

#ifndef RZMOTIONTIMER_H_
#define RZMOTIONTIMER_H_

class RzMotionTimer {
public:
	RzMotionTimer();
	virtual ~RzMotionTimer();

	float start;
	float rate;
};

#endif /* RZMOTIONTIMER_H_ */
