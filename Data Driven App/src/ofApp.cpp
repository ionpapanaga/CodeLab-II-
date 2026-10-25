#include "ofApp.h"

void ofApp::setup() {
	ofSetWindowTitle("Vulnerability Scanner - CodeLab II");
	ofBackground(24, 27, 34);

	// Load built-in system font or standard openFrameworks font at larger sizes
	// OF_TTF_MONO provides a clean technical/terminal look
	titleFont.load(OF_TTF_MONO, 18, true, true);
	headerFont.load(OF_TTF_MONO, 13, true, true);
	bodyFont.load(OF_TTF_MONO, 11, true, true);

	// Make ofxGui panel and controls wider and taller
	gui.setDefaultWidth(320);
	gui.setDefaultHeight(28);

	gui.setup("Scan Parameters", "settings.xml", 30, 40);
	gui.add(inputCountry.setup("Country Code", "US"));
	gui.add(inputProduct.setup("Host / Target", "1.1.1.1"));
	gui.add(scanBtn.setup("Run Scan"));

	scanBtn.addListener(this, &ofApp::onScanPressed);
}

void ofApp::onScanPressed() {
	if (isLoading) return;
	performScan(inputCountry, inputProduct);
}

void ofApp::performScan(std::string country, std::string target) {
	isLoading = true;
	statusMessage = "Contacting Shodan & InternetDB...";
	currentResult = ScanResult();

	std::string ip = target;

	// If target is a hostname, resolve via Shodan DNS API
	if (target.find_first_not_of("0123456789.") != std::string::npos) {
		std::string dnsUrl = "https://api.shodan.io/dns/resolve?hostnames=" + target + "&key=" + shodanApiKey;
		ofHttpResponse dnsResp = ofLoadURL(dnsUrl);

		if (dnsResp.status == 200) {
			try {
				auto dnsJson = json::parse(dnsResp.data.getText());
				if (dnsJson.contains(target) && !dnsJson[target].is_null()) {
					ip = dnsJson[target].get<std::string>();
				}
			} catch (...) {
				statusMessage = "Shodan DNS Parse Error";
				isLoading = false;
				return;
			}
		} else {
			statusMessage = "Shodan API Error: HTTP " + ofToString(dnsResp.status);
			isLoading = false;
			return;
		}
	}

	currentResult.ip = ip;

	// Query InternetDB
	std::string internetDbUrl = "https://internetdb.shodan.io/" + ip;
	ofHttpResponse dbResp = ofLoadURL(internetDbUrl);

	if (dbResp.status == 200) {
		try {
			auto dbJson = json::parse(dbResp.data.getText());
			currentResult.found = true;

			if (dbJson.contains("cves") && dbJson["cves"].is_array()) {
				for (const auto & cve : dbJson["cves"]) {
					currentResult.cves.push_back(cve.get<std::string>());
				}
				currentResult.vulnCount = (int)currentResult.cves.size();
			}

			if (dbJson.contains("ports") && dbJson["ports"].is_array()) {
				for (const auto & p : dbJson["ports"]) {
					currentResult.ports.push_back(p.get<int>());
				}
				currentResult.portCount = (int)currentResult.ports.size();
			}

			if (dbJson.contains("tags") && dbJson["tags"].is_array()) {
				for (const auto & t : dbJson["tags"]) {
					currentResult.tags.push_back(t.get<std::string>());
				}
			}

			statusMessage = "Scan Complete for " + ip;
		} catch (...) {
			statusMessage = "InternetDB parsing error";
		}
	} else if (dbResp.status == 404) {
		statusMessage = "No vulnerabilities recorded for " + ip;
		currentResult.found = true;
	} else {
		statusMessage = "InternetDB API error: HTTP " + ofToString(dbResp.status);
	}

	isLoading = false;
}

void ofApp::update() { }

void ofApp::draw() {
	// 1. Draw GUI sidebar
	gui.draw();

	int panelX = 400;
	int panelY = 40;
	int cardWidth = ofGetWidth() - panelX - 40;
	int cardHeight = ofGetHeight() - 80;

	// 2. Draw modern background card for results
	ofSetColor(34, 39, 49);
	ofDrawRectRounded(panelX, panelY, cardWidth, cardHeight, 10);

	// 3. Status Bar
	ofSetColor(isLoading ? ofColor(255, 193, 7) : ofColor(76, 175, 80));
	bodyFont.drawString("STATUS: " + statusMessage, panelX + 25, panelY + 35);

	// 4. Report Header
	ofSetColor(240);
	titleFont.drawString("SCAN REPORT", panelX + 25, panelY + 75);

	// Divider Line
	ofSetColor(60, 68, 85);
	ofSetLineWidth(2);
	ofDrawLine(panelX + 25, panelY + 90, panelX + cardWidth - 25, panelY + 90);

	// 5. Target Overview
	ofSetColor(200);
	headerFont.drawString("Target IP:    " + (currentResult.ip.empty() ? "-" : currentResult.ip), panelX + 25, panelY + 125);

	// Highlight vulnerability count
	if (currentResult.vulnCount > 0) {
		ofSetColor(255, 99, 99);
	} else {
		ofSetColor(120, 220, 120);
	}
	headerFont.drawString("Vulnerabilities (CVEs): " + ofToString(currentResult.vulnCount), panelX + 25, panelY + 155);

	ofSetColor(200);
	headerFont.drawString("Open Ports:  " + ofToString(currentResult.portCount), panelX + 25, panelY + 185);

	// 6. CVE List Breakdown
	ofSetColor(230);
	headerFont.drawString("Discovered CVEs:", panelX + 25, panelY + 230);

	int cveY = panelY + 260;
	if (currentResult.cves.empty()) {
		ofSetColor(140);
		bodyFont.drawString(currentResult.found ? "None detected (Clean)." : "Run a scan to view results.", panelX + 35, cveY);
	} else {
		ofSetColor(255, 120, 120);
		int maxItems = std::min((int)currentResult.cves.size(), 16);
		for (int i = 0; i < maxItems; ++i) {
			bodyFont.drawString("•  " + currentResult.cves[i], panelX + 35, cveY);
			cveY += 24;
		}
		if ((int)currentResult.cves.size() > 16) {
			ofSetColor(180);
			bodyFont.drawString("... and " + ofToString(currentResult.cves.size() - 16) + " additional CVEs", panelX + 35, cveY);
		}
	}
}

void ofApp::keyPressed(int key) { }
