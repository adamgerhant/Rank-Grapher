#pragma once
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include <iostream>
#include <cstdio>
#include <string>
#include <fstream>
#include <sstream>
#include "imgui/imgui.h"
#include "version.h"
#include "bakkesmod/wrappers/MMRWrapper.h"
#include "bakkesmod/wrappers/GuiManagerWrapper.h"
#include "utils/parser.h"
#include "version.h"

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

using namespace std;

class RankGrapher: public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow, public BakkesMod::Plugin::PluginWindow
{

	//std::shared_ptr<bool> enabled;

	//Boilerplate
	virtual void onLoad();
	virtual void onUnload();
	UniqueIDWrapper uniqueID;
	int yPos;

	std::shared_ptr<ImageWrapper> rankgrapherbg;
	std::shared_ptr<ImageWrapper> closeImg;

	ImVec4 color = ImVec4(1.0f, 0.65f, 0.0f, 1.0f);

	void onGameEntry();
	void gameEnd(string eventName);
	void RenderCanvas(CanvasWrapper canvas);
	void loadMenu(string eventName);
	void scoreboardClose(string eventName);
	void testing();
	void loadGraph();
	void scoreboardLoad(string eventName);
	void RenderImGui();

    bool startWindow = false;
	bool shouldHideCursor = false;
	bool shouldRenderImGui = false;
	bool isGameEnd = 0;
	float gameEndMMR = 0.0;
	bool gotNewMMR = false;
	float userMMR = 0;
	int currentSession = 0;
	int gameSessionId;
	int playlistNum = -1;
	bool isRanked = 0;

	int goals = -1;
	int assists = -1;
	int saves = -1;
	int demos = -1;
	int shots = -1;
	int score = -1;
	long mvp = 0;
	bool win = false;

	float xOffset;
    float yOffset;
    Vector2 screenSize;
	string currentMatchGuid;
	string fileName;
	string fileNameTemp;
	// Inherited via PluginSettingsWindow
	
	void RenderSettings() override;
	std::string GetPluginName() override;
	void SetImGuiContext(uintptr_t ctx) override;
	

	// Inherited via PluginWindow
	

	bool isWindowOpen_ = false;
	bool isMinimized_ = false;
	std::string menuTitle_ = "RankGrapher plugin";

	virtual void Render() override;
	virtual std::string GetMenuName() override;
	virtual std::string GetMenuTitle() override;
	//virtual void SetImGuiContext(uintptr_t ctx) override;
	virtual bool ShouldBlockInput() override;
	virtual bool IsActiveOverlay() override;
	virtual void OnOpen() override;
	virtual void OnClose() override;
	
	
};

