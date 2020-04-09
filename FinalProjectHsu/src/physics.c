/*
 * physics.c
 *
 *  Created on: Apr 1, 2020
 *      Author: Andy Hsu
 */

#include "physics.h"


struct Vehicle vehicle;
struct RoadLayout Road;
Waypoints waypoint[100];

void Road_init(char *name, int waypoints[], int numwaypoints)
{
	char *RoadName;
	if(strlen(name) > 15)
		RoadName = "SpaR";
	else
		RoadName = name;

	for(int i = 0; i < strlen(RoadName); i++)
	{
		Road.Name[i] = RoadName[i];
	}

	Road.RoadWidth = 10;

	if(Road.RoadWidth < 5 || Road.RoadWidth > 50)
		Road.RoadWidth = 10;

	if((sizeof(waypoints)/sizeof(int))/2 != numwaypoints)
	{
		uint16_t SpaR2[] = {10, 0,
							0,  0,
							10, 0,
							20, 0,
							50, 10,
							80, 5,
							110, 10,
							130, 20,
							137, 40,
							139, 55,
							140, 80,
							135, 110,
							135, 190,
							150, 220,
							170, 225,
							190, 225,
							205, 210,
							207, 200,
							205, 200,
							150, 100,
							148, 90,
							149, 75,
							165, 50,
							200, 40,
							210, 38,
							230, 39};

		for(int i = 0; i < sizeof(SpaR2)/sizeof(int); i++)
		{
			if(i % 2 == 0)
				waypoint[i].x = SpaR2[i];
			else
				waypoint[i].y = SpaR2[i];
		}
		Road.NumWaypoints = (sizeof(SpaR2)/sizeof(int))/2;
	}
	else
	{
		for(int i = 0; i < sizeof(waypoints)/sizeof(int); i++)
		{
			if(i % 2 == 0)
				waypoint[i].x = waypoints[i];
			else
				waypoint[i].y = waypoints[i];
		}
		Road.NumWaypoints = numwaypoints;
	}

	Road.wayarray = &waypoint;

}


void Vehicle_init(char *Name, int mass, int maxpower, int turnradius, int vehiclewidth, int dragarea, int tiretype)
{
	char *VehicleName;
	if(strlen(Name) > 16)
		VehicleName = "Clunker";
	else
		VehicleName = Name;
	for(int i = 0; i < strlen(VehicleName); i++)
	{
<<<<<<< HEAD
		vehicle.VehicleName[i] = VehicleName[i];
	}

	vehicle.Mass = mass;
	vehicle.MaxPower = maxpower;
	vehicle.TurnRadius = turnradius;
	vehicle.VehcileWidth = vehiclewidth;
	vehicle.DragArea = dragarea;
	vehicle.TireType = tiretype;

	if(vehicle.Mass < 200 || vehicle.Mass > 10000)
		vehicle.Mass = 1234;
	if(vehicle.MaxPower < 50 || vehicle.MaxPower > 1000)
		vehicle.MaxPower = 93;
	if(vehicle.TireType < 0 || vehicle.TireType > 2)
		vehicle.TireType = TOURISM;
	if(vehicle.TurnRadius < 0 || vehicle.TurnRadius > 1100)
		vehicle.TurnRadius = 595;
	if(vehicle.VehcileWidth < 100 || vehicle.VehcileWidth > 300)
		vehicle.VehcileWidth = 170;
	if(vehicle.DragArea < 100 || vehicle.DragArea > 1200)
		vehicle.DragArea = 616;
=======
		Vehicle.VehicleName[i] = VehicleName[i];
	}

	Vehicle.Mass = mass;
	Vehicle.MaxPower = maxpower;
	Vehicle.TurnRadius = turnradius;
	Vehicle.VehcileWidth = vehiclewidth;
	Vehicle.DragArea = dragarea;
	Vehicle.TireType = tiretype;

	if(Vehicle.Mass < 200 || Vehicle.Mass > 10000)
		Vehicle.Mass = 1234;
	if(Vehicle.MaxPower < 50 || Vehicle.MaxPower > 1000)
		Vehicle.MaxPower = 93;
	if(Vehicle.TireType < 0 || Vehicle.TireType > 2)
		Vehicle.TireType = TOURISM;
	if(Vehicle.TurnRadius < 0 || Vehicle.TurnRadius > 1100)
		Vehicle.TurnRadius = 595;
	if(Vehicle.VehcileWidth < 100 || Vehicle.VehcileWidth > 300)
		Vehicle.VehcileWidth = 170;
	if(Vehicle.DragArea < 100 || Vehicle.DragArea > 1200)
		Vehicle.DragArea = 616;
>>>>>>> ab31d91841fb264cef8f4b309c0572fc948e724a

}


void pylon_data(Waypoints prev, Waypoints curr, Waypoints* abspylon1, Waypoints* abspylon2)
{
	float x1 = prev.x;
	float y1 = prev.y;

	float x2 = curr.x;
	float y2 = curr.y;

	float HalfWidth = Road.RoadWidth/2;

	if((x2-x1) == 0)
	{
		abspylon1->x = curr.x + (HalfWidth);
		abspylon1->y = curr.y;

		abspylon2->x = curr.x - (HalfWidth);
		abspylon2->y = curr.y;
		return;
	}


	float currentSlope = (y2-y1)/(x2-x1);

	float normalSlope = -(1/currentSlope);

	float relativex;
	float relativey;

	float changeinx;
	float changeiny;

	if(currentSlope == 0)
	{
		abspylon1->y = curr.y + (HalfWidth);
		abspylon1->x = curr.x;

		abspylon2->y = curr.y - (HalfWidth);
		abspylon2->x = curr.x;
		return;
	}

	for(int i = 0; i <= HalfWidth; i++)
	{

		relativex = curr.x + i;
		changeinx = curr.x - relativex;

		relativey = ((normalSlope)*(relativex - curr.x)) + curr.y;
		changeiny = curr.y - relativey;

		if(((HalfWidth*HalfWidth)-((changeinx*changeinx)+(changeiny*changeiny)))<0.1)
		{
			abspylon1->x = relativex;
			abspylon1->y = relativey;
			break;
		}
	}

	abspylon2->x = curr.x + changeinx;
	abspylon2->y = curr.y + changeiny;
}
