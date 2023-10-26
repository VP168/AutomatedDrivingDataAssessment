#pragma once

# define M_PI           3.141592f
# define M_PI_2         1.570796f
# define M_PI_4         0.785398f

struct DataAssessmentParameters {
	bool checkObjectIdentification = false;
	bool checkCarNearbyCondition = true;
	bool checkPedestrianNearbyCondition = false;
	bool checkCyclistNearbyCondition = true;


	float minimumAllowedFrontDistanceToObject = 3.0f; //meters
	float minimumAllowedRearDistanceToObject = 1.5f;
	float minimumAllowedLateralDistanceToObject = 2.0f;

	float lowSpeedLimit = 3.0f; //mps

	float egoRearAxleToFront = 2.0f;  //meters
	float egoRearAxleToRear = 1.0f;
	float egoWidth = 2.0f;
};

enum class ObjectLocation {
	UNKNOWN,
	FRONT,
	BEHIND,
	LEFT,
	RIGHT
};

class DataAssessment {
private: 
	float distanceToEgo;
	float headingAngleDifference;
	Point2D transformedObject;

	void calculateDistanceToEgo(Point2D transformedObject) {
		auto squaredDist = std::pow(transformedObject.x, 2) + std::pow(transformedObject.y, 2);
		distanceToEgo = static_cast<float>(std::sqrt(squaredDist));
	}

	void calculateHeadingAngleDifference(Point2D transformedObject) {
		headingAngleDifference = transformedObject.yaw;
	}

public:
	float getDistanceToEgo(Point2D transformedObject) {
		calculateDistanceToEgo(transformedObject);
		return distanceToEgo;
	}

	ObjectLocation getObjectLocation(float headingAngleDifference, Point2D transformedObject) {
		ObjectLocation objLocation (ObjectLocation::UNKNOWN);
		DataAssessmentParameters parameters;
		if (   std::abs(headingAngleDifference) < M_PI_4
			&& transformedObject.x > parameters.minimumAllowedFrontDistanceToObject
			&& transformedObject.y < parameters.egoWidth) {
			objLocation = ObjectLocation::BEHIND;
		}
		else if (std::abs(headingAngleDifference) < M_PI_4
			&& transformedObject.x < parameters.minimumAllowedRearDistanceToObject
			&& transformedObject.y < parameters.egoWidth) {
			objLocation = ObjectLocation::FRONT;
		}
		else if (transformedObject.y > parameters.minimumAllowedLateralDistanceToObject
			&& transformedObject.x > parameters.minimumAllowedRearDistanceToObject
			&& transformedObject.x < parameters.minimumAllowedFrontDistanceToObject) {

			if ((headingAngleDifference > M_PI_4 && headingAngleDifference < (M_PI_2 + M_PI_4))
				|| headingAngleDifference > (M_PI + M_PI_4) && headingAngleDifference < (2 * M_PI - M_PI_4)) {
				objLocation = ObjectLocation::LEFT;
			}
		}
		else if (transformedObject.y < parameters.minimumAllowedLateralDistanceToObject
				&& transformedObject.x > parameters.minimumAllowedRearDistanceToObject
				&& transformedObject.x < parameters.minimumAllowedFrontDistanceToObject) {

				if ((headingAngleDifference > M_PI_4 && headingAngleDifference < (M_PI_2 + M_PI_4))
					|| headingAngleDifference > (M_PI + M_PI_4) && headingAngleDifference < (2 * M_PI - M_PI_4)) {
					objLocation = ObjectLocation::RIGHT;
				}
		
			}
		else {
			objLocation = ObjectLocation::UNKNOWN;
		}

		return objLocation;
	}
};