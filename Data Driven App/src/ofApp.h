#pragma once

#include "json.hpp"
#include "ofMain.h"
#include "ofxGui.h"

using json = nlohmann::json;

struct ScanResult {
	std::string ip;
	int portCount = 0;
	int vulnCount = 0;
	std::vector<std::string> cves;
	std::vector<int> ports;
	std::vector<std::string> tags;
	bool found = false;
};

class ofApp : public ofBaseApp {
public:
	void setup() override;
	void update() override;
	void draw() override;
	void keyPressed(int key) override;

	// GUI Controls
	ofxPanel gui;
	ofxInputField<std::string> inputCountry;
	ofxInputField<std::string> inputProduct;
	ofxButton scanBtn;

	// Fonts for scaled rendering
	ofTrueTypeFont titleFont;
	ofTrueTypeFont headerFont;
	ofTrueTypeFont bodyFont;

	// Scan handling
	void onScanPressed();
	void performScan(std::string country, std::string target);

	// App state
	std::string shodanApiKey = "5ENHPHgSQbhtlepJpalg429KMTbedCrF";
	std::string statusMessage = "Ready";
	bool isLoading = false;
	ScanResult currentResult;
};
