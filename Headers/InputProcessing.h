#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include "Dependencies/json.hpp"



struct EgoSceneData {
	size_t sceneId = 0;
	float x = 0.0f;
	float y = 0.0f;
	float speed = 0.0f;
	float yaw = 0.0f;
};

enum class ObjectType {
	NOT_DEFINED,
	UNKNOWN,
	PEDESTRIAN,
	CYCLIST,
	VEHICLE_PASSENGER_CAR
};

struct ObjectSceneData {
	size_t sceneId = 0;
	size_t objectId = 0;
	float x = 0.0f;
	float y = 0.0f;
	float speed = 0.0f;
	float yaw = 0.0f;
	float width = 0.0f;
	float length = 0.0f;
	ObjectType objType = ObjectType::NOT_DEFINED;

	bool operator < (const ObjectSceneData& scene1) {
		return sceneId < scene1.sceneId;
	}
};

class InputData {
private:
	nlohmann::json jsonData;
	std::vector<EgoSceneData> egoData;
	std::vector<ObjectSceneData> objectObservations;

	nlohmann::json getEgoJsonData() {
		auto egoIterator = jsonData.find("ego");
		if (egoIterator == jsonData.end())
		{
			std::cout << "Ego data is Empty! ";
			return { };
		}

		return jsonData["ego"];
	}

	nlohmann::json getObjectJsonData() {
		auto objectIterator = jsonData.find("obs");
		if (objectIterator == jsonData.end())
		{
			std::cout << "Observations data is Empty! ";
			return { };
		}

		return jsonData["obs"];
	}

	ObjectType set_object_type(std::string objType) {
		ObjectType objectType;
		if (objType == "VEHICLE_PASSENGER_CAR") {
			objectType = ObjectType::VEHICLE_PASSENGER_CAR;
		}
		else if (objType == "CYCLIST") {
			objectType = ObjectType::CYCLIST;
		}
		else if (objType == "PEDESTRIAN") {
			objectType = ObjectType::PEDESTRIAN;
		}
		else if (objType == "UNKNOWN") {
			objectType = ObjectType::UNKNOWN;
		}
		else {
			objectType = ObjectType::NOT_DEFINED;
		}

		return objectType;
	}

public:

	void readJsonData() {
		std::ifstream jsonFileStream("./Dependencies/DrivingData/driving_data.json");
		jsonData = nlohmann::json::parse(jsonFileStream);
	}

	std::vector<EgoSceneData> getEgoData() {
		egoData.clear();
		nlohmann::json egoJsonData = getEgoJsonData();
		auto egoJsonDataIterator = egoJsonData.find("scene_id");
		if (egoJsonDataIterator== egoJsonData.end()) {
			return egoData;
		}

		size_t index = 0;
		for (auto it = egoJsonData["scene_id"].begin(); it != egoJsonData["scene_id"].end(); ++it) {
			EgoSceneData egoSceneData;
			egoSceneData.sceneId = egoJsonData["scene_id"].at(index);
			egoSceneData.x = egoJsonData["x"].at(index);
			egoSceneData.y = egoJsonData["y"].at(index);
			egoSceneData.speed = egoJsonData["speed"].at(index);
			egoSceneData.yaw = egoJsonData["yaw"].at(index);
			index++;

			egoData.emplace_back(egoSceneData);
		}

		return egoData;
	}

	std::vector<ObjectSceneData> getObservations() {
		objectObservations.clear();
		nlohmann::json objJsonObservations = getObjectJsonData();
		if (objJsonObservations.size() == 0) {
			return objectObservations;
		}

		for (auto it = objJsonObservations.begin(); it != objJsonObservations.end(); ++it) {
			ObjectSceneData objectSceneData;
			objectSceneData.objectId =static_cast<size_t>(std::stoi(it.key()));

			size_t index = 0;
			for (auto objIt = it.value()["scene_id"].begin(); objIt != it.value()["scene_id"].end(); ++objIt) {
				objectSceneData.sceneId = it.value()["scene_id"].at(index);
				objectSceneData.x = it.value()["x"].at(index);
				objectSceneData.y = it.value()["y"].at(index);
				objectSceneData.speed = it.value()["speed"].at(index);
				objectSceneData.yaw = it.value()["yaw"].at(index);
				objectSceneData.width = it.value()["width"].at(index);
				objectSceneData.length = it.value()["length"].at(index);
				objectSceneData.objType = set_object_type(it.value()["type"].at(index));
				index++;
				objectObservations.emplace_back(objectSceneData);
			}			
		}

		return objectObservations;

	}


	std::vector<ObjectSceneData> getObservationsSortedBySceneId() {
		auto sortedObservations = getObservations();
		std::sort(sortedObservations.begin(), sortedObservations.end());
		return sortedObservations;
	}

	std::vector<ObjectSceneData> getObservationsAsPerSceneID(std::vector<ObjectSceneData> & sortedObservations, size_t sceneId) {
		std::vector<ObjectSceneData> observationsAsPerSceneId;
		for (auto& eachObject : sortedObservations) {
			if (eachObject.sceneId == sceneId) {
				observationsAsPerSceneId.emplace_back(eachObject);
			}
		}
		return observationsAsPerSceneId;
	}
};