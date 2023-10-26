#pragma once

struct Point2D {
	float x = 0.0f;
	float y = 0.0f;
	float yaw = 0.0f;
};

class Transformator {
private:
	Point2D origin;
	Point2D object;
	Point2D transformedObject;

	void calculateTransformation() {
		// Translate object to the origin
		auto dx = object.x - origin.x;
		auto dy = object.y - origin.y;

		// Rotate object around the origin by angle yaw
		auto xObjectRotated = dx * cos(object.yaw) - dy * sin(object.yaw);
		auto yObjectRotated = dx * sin(object.yaw) + dy * cos(object.yaw);

		// Translate object back to its original position
		auto xObjectTranslated = xObjectRotated + origin.x;
		auto yObjectTranslated = xObjectRotated + origin.y;

		// Translate new translated object to the origin
		auto dxNew = xObjectTranslated - origin.x;
		auto dyNew = yObjectTranslated - origin.y;

		// Transformed object position
		transformedObject.x = dxNew * cos(origin.yaw) - dyNew * sin(origin.yaw);
		transformedObject.y = dxNew * sin(origin.yaw) + dyNew * cos(origin.yaw);

		// Transformed object rotation angle
		transformedObject.yaw = object.yaw + origin.yaw;
	}

public:
	void setEgoOrigin(float xO, float yO, float yaw) {
		origin.x = xO;
		origin.y = yO;
		origin.yaw= yaw;
	}

	void setObjectLocationToTransform(float x, float y, float yaw) {
		object.x = x;
		object.y= y;
		object.yaw = yaw;
	}

	Point2D getTransforedObjectLocation() {
		calculateTransformation();
		return transformedObject;
	}

};