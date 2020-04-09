/*
 * physics.h
 *
 *  Created on: Apr 1, 2020
 *      Author: Andy Hsu
 */

#include <stdint.h>
#include <string.h>

#ifndef SRC_PHYSICS_H_
#define SRC_PHYSICS_H_

uint16_t refresh_pov;
uint16_t update_physics;
uint16_t prcip;
uint16_t atmos_density;
uint16_t driver_expierence;

//uint16_t SpaR2[];

struct vehicle_info_struct
{
	char VehicleName[16];	// not null terminated
	uint16_t Mass;			// kg
	uint16_t MaxPower;		// kW
	uint16_t TurnRadius;	// cm, curb-to-curb
	uint16_t VehcileWidth;  // cm

	// Vehicle characteristics for slippage
	uint16_t DragArea;		// Cd*CrossSectionalArea as: Cd*Ft^2*100
	uint16_t TireType;		// 0: truck, 1: tourism, 2: high performance
};
// Note: for road-excursion calculations, vehicle is assumed to have a circular footprint


struct RoadLayout
{
	char Name[15];
	uint16_t RoadWidth;		    // cm
	//uint16_t MaxBank; 		    // degress
	//uint16_t CompensatedSpeed;  // m/s. The speed at which the road should be banked for
								// optimum driving, up to the MaxBank. (See info section)

	uint16_t NumWaypoints;

	struct Waypoints* wayarray; // Array of waypoints. First and last 2 are not to be driven

};

typedef struct Waypoints
{
	float x;			// m
	float y; 			// m
} Waypoints;

// Road composition
enum
{
	ASPHALT,
	EARTH,
	GRAVEL,
	SAND,
	SNOWPACK,
	ICE,
};

// Tire Type
enum
{
	TRUCK,
	TOURISM,
	HIGHPERFORMANCE,
};



/*
 * description: creates the default values for the Road and adds the road layout to the waypoint
 * array
 * parameters: none
 * returns: none
 */
void Road_init(char *name, int waypoints[], int numwaypoints);

/*
 * description:
 * parameters:
 * returns: none
 */
void Vehicle_init(char *Name, int mass, int maxpower, int turnradius, int vehiclewidth, int dragarea, int tiretype);

/*
 * description: Calculates and determines spots for pylons based on a given waypoints
 * parameters: Waypoints prev- previous waypoint, Waypoints curr- current waypoint,
 * 				Waypoints* abspylon1- first pylon, Waypoints* abspylon2- second pylon
 * returns: none
 */
void pylon_data(Waypoints prev, Waypoints curr, Waypoints* abspylon1, Waypoints* abspylon2);

#endif /* SRC_PHYSICS_H_ */
