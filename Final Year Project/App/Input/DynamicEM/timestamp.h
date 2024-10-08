/*
 * timestamp.h
 *
 *  Created on: 29 May 2015
 *      Author: arunjayapal
 */

#include <sys/time.h>
typedef unsigned long long timestamp;

#ifndef TIMESTAMP_H_
#define TIMESTAMP_H_
static timestamp get_timestamp() {
	struct timeval now;
	gettimeofday(&now, NULL);
	return now.tv_usec + (timestamp) now.tv_sec * 1000000;
}
#endif /* TIMESTAMP_H_ */
