#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "Dependencies/json.hpp"

struct AssessmentDataOutput {
	size_t sceneId = 0;
	size_t objectId = 0;
	std::string remark = "";
};

class OutputProcessing {
private:
	nlohmann::json jsonOutputData = nlohmann::json::array();
public:
	nlohmann::json setAssessmentData(std::vector<AssessmentDataOutput> assessmentData ) {
		for (const auto& each : assessmentData) {
			jsonOutputData.push_back({ {"scene_id", each.sceneId}, {"object_id", each.objectId}, {"observation", each.remark} });
		}
		return jsonOutputData;
	}

	void getAssessmentDataJson() {
		std::ofstream jsonOutput("./OutputData/DataAssessmentOutput.json");
		jsonOutput << jsonOutputData << std::endl;
	}
};