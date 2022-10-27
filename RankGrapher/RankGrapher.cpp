#include "pch.h"
#include "RankGrapher.h"




BAKKESMOD_PLUGIN(RankGrapher, "Rank Grapher", plugin_version, PLUGINTYPE_FREEPLAY)


std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void RankGrapher::onLoad()
{
	_globalCvarManager = cvarManager;

    isWindowOpen_ = true;
    xOffset = -170 * ((float)screenSize.X / 1920);
    yOffset = 90 * ((float)screenSize.Y / 1080);
    gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {
        RenderCanvas(canvas);
        });


    shouldHideCursor = false;
    shouldRenderImGui = true;

	gameWrapper->SetTimeout([this](GameWrapper* gameWrapper) {
		cvarManager->executeCommand("openmenu " + GetMenuName());
	}, 1);
	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.WaitingForPlayers.EndState", [this](std::string eventName) {
		onGameEntry();
	});
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.Destroyed", bind(&RankGrapher::loadMenu, this, std::placeholders::_1));
    



	rankgrapherbg = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "RankGrapher" / "ranktrackerbg.png", true, false);
	closeImg = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "RankGrapher" / "closebutton.png", false, true);
	

	//auto cvar = cvarManager->registerCvar("template_cvar", "hello-cvar", "just a example of a cvar");
	cvarManager->registerCvar("timeframe", "all", "timeframe variable", true);
	cvarManager->registerCvar("displayOnScoreboard", "1", "displays on scoreboard", true);
	cvarManager->registerCvar("displayAfterGame", "1", "displays after game", true);
	cvarManager->registerCvar("enableSmoothness", "0", "enables custom smoothness", true);
	cvarManager->registerCvar("smoothness", "1", "smoothness of curve", true);

    CVarWrapper displayOnScoreboardCVar = cvarManager->getCvar("displayOnScoreboard");
    if (displayOnScoreboardCVar) {
        bool displayOnScoreboard = displayOnScoreboardCVar.getBoolValue();
        if (displayOnScoreboard) {
            gameWrapper->HookEvent("Function TAGame.GFxData_GameEvent_TA.OnOpenScoreboard", bind(&RankGrapher::scoreboardLoad, this, std::placeholders::_1));
            gameWrapper->HookEvent("Function TAGame.GFxData_GameEvent_TA.OnCloseScoreboard", bind(&RankGrapher::CloseWindow, this, std::placeholders::_1));
        }

    }
    CVarWrapper displayAfterGameCVar = cvarManager->getCvar("displayAfterGame");
    if (displayAfterGameCVar) {
        bool displayAfterGame = displayAfterGameCVar.getBoolValue();
        if (displayAfterGame) {
            gameWrapper->HookEvent("Function GameEvent_Soccar_TA.PodiumSpotlight.EndState", bind(&RankGrapher::gameEnd, this, std::placeholders::_1));
        }

    }
    //saves session data to file
	ofstream outputFile;
	ifstream inputFile;
	string line = "";
	string tempString;
	int sessionNumber = 0;
	
	inputFile.open(gameWrapper->GetDataFolder()/ "RankGrapher" / "session.csv");

	

	if (inputFile.fail()) {
		outputFile.open(gameWrapper->GetDataFolder() / "RankGrapher" / "session.csv");
		outputFile << "1" <<"\n";
		outputFile.close();
		inputFile.close();
		currentSession = 1;
	}

	else {
		outputFile.open(gameWrapper->GetDataFolder() / "RankGrapher" / "session_temp.csv");
		while (getline(inputFile, line)) {



			stringstream inputString(line);

			getline(inputString, tempString, ',');
			sessionNumber = atoi(tempString.c_str());
			

			//outputFile << to_string(sessionNumber) << "\n";

			line = "";


		}

		currentSession = sessionNumber + 1;
		outputFile << to_string(currentSession) << "\n";
		outputFile.close();
		inputFile.close();

		remove(gameWrapper->GetDataFolder() / "RankGrapher" / "session.csv");


		// renaming the updated file with the existing file name
		rename(gameWrapper->GetDataFolder() / "RankGrapher" / "session_temp.csv", gameWrapper->GetDataFolder() / "RankGrapher" / "session.csv");
	}

	
	
	//cvar.addOnValueChanged([this](std::string cvarName, CVarWrapper newCvar) {
	//	cvarManager->log("the cvar with name: " + cvarName + " changed");
	//	cvarManager->log("the new value is:" + newCvar.getStringValue());
	//});

	//cvar2.addOnValueChanged(std::bind(&RankGrapher::YourPluginMethod, this, _1, _2));

	// enabled decleared in the header
	//enabled = std::make_shared<bool>(false);
	//cvarManager->registerCvar("TEMPLATE_Enabled", "0", "Enable the TEMPLATE plugin", true, true, 0, true, 1).bindTo(enabled);

	//cvarManager->registerNotifier("NOTIFIER", [this](std::vector<std::string> params){FUNCTION();}, "DESCRIPTION", PERMISSION_ALL);
	//cvarManager->registerCvar("CVAR", "DEFAULTVALUE", "DESCRIPTION", true, true, MINVAL, true, MAXVAL);//.bindTo(CVARVARIABLE);
	//gameWrapper->HookEvent("FUNCTIONNAME", std::bind(&TEMPLATE::FUNCTION, this));
	//gameWrapper->HookEventWithCallerPost<ActorWrapper>("FUNCTIONNAME", std::bind(&RankGrapher::FUNCTION, this, _1, _2, _3));
	//gameWrapper->RegisterDrawable(bind(&TEMPLATE::Render, this, std::placeholders::_1));


	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", [this](std::string eventName) {
	//	cvarManager->log("Your hook got called and the ball went POOF");
	//});
	// You could also use std::bind here
	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", std::bind(&RankGrapher::YourPluginMethod, this);
}

void RankGrapher::onUnload()
{
	//gameWrapper->UnhookEvent("Function TAGame.GameEvent_Soccar_TA.OnMatchWinnerSet");
	//gameWrapper->UnhookEvent("Function TAGame.GameEvent_Soccar_TA.Destroyed");
	//gameWrapper->UnhookEvent("Function TAGame.GFxData_MenuStack_TA.ButtonTriggered");
	//gameWrapper->UnregisterDrawables();
}

void RankGrapher::RenderCanvas(CanvasWrapper canvas) {
	/*
	if (!gameWrapper->IsInOnlineGame()) {
		gameWrapper->UnregisterDrawables();
	}
	*/

    screenSize = gameWrapper->GetScreenSize();
	if (rankgrapherbg->IsLoadedForCanvas()) {
		
		canvas.SetPosition(Vector2F{ 475.0f  /*((float)screenSize.X / 1920)*/ + xOffset,  549.0f  /*((float)screenSize.X / 1920)*/ + yOffset});
		canvas.DrawTexture(rankgrapherbg.get(), 1);
	}
	
    
    //LOG("{}", screenSize.Y);
	float yBottom = 809*((float)screenSize.Y / 1080) + yOffset;
	float yTop = 693 * ((float)screenSize.Y / 1080) + yOffset;
	float xLeft = 834 * ((float)screenSize.X / 1920) + xOffset;
	float xRight = 1443 * ((float)screenSize.X / 1920) + xOffset;

	LinearColor colors;
	colors.R = 220;
	colors.G = 220;
	colors.B = 220;
	colors.A = 255;
	canvas.SetColor(colors);

	canvas.DrawLine(Vector2F{ xLeft, yBottom }, Vector2F{ xLeft, yTop }, 2);
	canvas.DrawLine(Vector2F{ xLeft, yBottom }, Vector2F{ xRight, yBottom }, 2);


	ifstream inputFile;

	inputFile.open(gameWrapper->GetDataFolder() / "RankGrapher" / (to_string(playlistNum) + "data.csv"));

	int gameNumber = 0;
	string gameId;
	float mmr;
	string tempString;
	string line = "";
	vector<float> allMMR;

	CVarWrapper timeframeCVar = cvarManager->getCvar("timeframe");
	if (!timeframeCVar) { return; }
	string timeframe = timeframeCVar.getStringValue();
	
	if (timeframe == "all") {
		while (getline(inputFile, line)) {

			stringstream inputString(line);

			getline(inputString, tempString, ',');
			gameNumber = atoi(tempString.c_str());

			getline(inputString, gameId, ',');

			getline(inputString, tempString, ',');
			mmr = atof(tempString.c_str());
			allMMR.push_back(mmr);

			line = "";
		
		}
	}
	if (timeframe == "session") {
		while (getline(inputFile, line)) {

			stringstream inputString(line);

			getline(inputString, tempString, ',');
			gameNumber = atoi(tempString.c_str());

			getline(inputString, gameId, ',');

			getline(inputString, tempString, ',');
			mmr = atof(tempString.c_str());

			getline(inputString, tempString, ',');
			int gameSessionId = atoi(tempString.c_str());

			//LOG("game session {}", gameSessionId);
			//LOG("current session {}", currentSession);

			if (gameSessionId == currentSession) {
				allMMR.push_back(mmr);
			}


			line = "";
			//LOG("session {}", mmr);
		}
	}

   
    if (gameWrapper->IsInOnlineGame()) {
        MMRWrapper mw = gameWrapper->GetMMRWrapper();
        int userPlaylist = mw.GetCurrentPlaylist();
    }
	UniqueIDWrapper uniqueID = gameWrapper->GetUniqueID();

    //LOG("{}", userPlaylist2);
	gotNewMMR = false;
	

	if (isGameEnd) {
		for (int i = 0; i < 10; i++) {
		
			if (gameWrapper->GetMMRWrapper().IsSynced(uniqueID, userPlaylist) && !gameWrapper->GetMMRWrapper().IsSyncing(uniqueID)) {

				// Makes sure it is one of the ranked gamemodes to prevent crashes
				if (!(find(begin(rankedPlaylists), end(rankedPlaylists), userPlaylist) != end(rankedPlaylists))) {
					return;
				}

				// Getting the mmr
				gameEndMMR = gameWrapper->GetMMRWrapper().GetPlayerMMR(uniqueID, userPlaylist);
				if (abs(gameEndMMR - mmr) < 50) {
					gotNewMMR = true;
					i = 10;
			
					allMMR.push_back(gameEndMMR);
				}
				
			}

		}
		
	}

	if (allMMR.size() == 0) {
		return;
	}


	float smallest_element = allMMR[0]; //let, first element is the smallest one
	float largest_element = allMMR[0]; //also let, first element is the biggest one

	for (int i = 1; i < allMMR.size(); i++)  //start iterating from the second element
	{
		if (allMMR[i] < smallest_element)
		{
			smallest_element = allMMR[i];
		}
		if (allMMR[i] > largest_element)
		{
			largest_element = allMMR[i];
		}
	}


	//LOG("{}", smallest_element);
	float range = largest_element - smallest_element;

	int step;

	if (range <= 100) {
		step = 25;
	}
	else if (range <= 250) {
		step = 50;
	}
	else if (range <= 400) {
		step = 75;
	}
	else {
		step = 100;
	}


	int minVal = ((int)smallest_element / step) * step;
	int maxVal;

	int remainder = (int)largest_element % step;
	if (remainder == 0) {
		maxVal = largest_element;
	}
	else {
		maxVal = largest_element + step - remainder;
	}

	float scaleRange = maxVal - minVal;

	float totalCount = (maxVal - minVal) / step;

	int count = 0;
	float xVal;
	float lineYVal;
	int val = minVal;

	while (val <= maxVal) {
		if (val >= 1000) {

			xVal = (int)xLeft - 37;
		}
		else {
			xVal = (int)xLeft - 27;
		}

		lineYVal = (int)yBottom - (val - minVal) / scaleRange * (yBottom - yTop);

		canvas.SetPosition(Vector2{ (int)xVal , (int)yBottom - (int)((yBottom - yTop) * count / (totalCount)) - 8 });

		colors.R = 220;
		colors.G = 220;
		colors.B = 220;
		colors.A = 255;
		canvas.SetColor(colors);
		canvas.DrawString(to_string(val), 1.0, 1.0, false);

		colors.R = 155;
		colors.G = 155;
		colors.B = 155;
		colors.A = 255;
		canvas.SetColor(colors);
		canvas.DrawLine(Vector2F{ xLeft, lineYVal }, Vector2F{ xRight, lineYVal }, 1);
		//LOG("{} {}", to_string((int)xLeft - 10), to_string((int)yBottom - (int)(yBottom - yTop) * count / totalCount));
		val += step;
		count++;
	}
   
	int xStep = allMMR.size() / 6 + 1;
	int xCount = 1;
	colors.R = 220;
	colors.G = 220;
	colors.B = 220;
	colors.A = 255;
	canvas.SetColor(colors);
	while (xCount <= allMMR.size()) {
		canvas.SetPosition(Vector2{ (int)xLeft + (int)((xRight - xLeft) * (xCount - 1) / (allMMR.size() - 1) * 0.99), (int)yBottom + 4 });
		canvas.DrawString(to_string(xCount), 1.0, 1.0, false);
		xCount += xStep;
	}
	//canvas.SetPosition(Vector2{ (int)xRight-20 , (int)yBottom+3 });
	//canvas.DrawString(to_string(allMMR.size()), 1.0, 1.0, false);

	float yVal1 = (int)yBottom - (int)(allMMR[0] - minVal) / scaleRange * (yBottom - yTop);
	float yVal2 = (int)yBottom - (int)(allMMR[0] - minVal) / scaleRange * (yBottom - yTop);
	float xVal1 = xLeft;
	float xVal2;

	if (allMMR.size() == 1) {
		xVal1 = xLeft;
		xVal2 = xRight;
        colors.R = color.x * 255;
        colors.G = color.y * 255;
        colors.B = color.z * 255;
        colors.A = color.w * 255;
		canvas.SetColor(colors);
		canvas.DrawLine(Vector2F{ xVal1, yVal1 }, Vector2F{ xVal2, yVal2 }, 2);

		colors.R = 220;
		colors.G = 220;
		colors.B = 220;
		colors.A = 255;
		canvas.SetColor(colors);

		canvas.SetPosition(Vector2{ (int)xRight, (int)yBottom + 4 });
		canvas.DrawString("1", 1.0, 1.0, false);
	}
	int iStep = 1;
    
	CVarWrapper enableSmoothnessCVar = cvarManager->getCvar("enableSmoothness");
    if (timeframeCVar) { //nullcheck
        if (enableSmoothnessCVar.getBoolValue()) {
            CVarWrapper smoothnessCVar = cvarManager->getCvar("smoothness");
            if (smoothnessCVar) {//nullcheck
                iStep = smoothnessCVar.getIntValue();
            }
            else {
                iStep = 1 + (allMMR.size() / 200);
            }
        }
    }
  
	for (int i = 1; i < allMMR.size(); i+=iStep) {
		yVal2 = (int)yBottom - (int)(allMMR[i] - minVal) / scaleRange * (yBottom - yTop);
		xVal2 = (int)xLeft + (int)((xRight - xLeft) * i / (allMMR.size() - 1));

		//LOG("{}", yVal2);
		LinearColor colors;
		colors.R = color.x*255;
		colors.G = color.y * 255;
		colors.B = color.z * 255;
		colors.A = color.w * 255;
		canvas.SetColor(colors);
		canvas.DrawLine(Vector2F{ xVal1-1, yVal1 }, Vector2F{ xVal2+1 , yVal2 }, 2);
		//canvas.DrawLine(Vector2F{ xVal2 , yVal1 }, Vector2F{ xVal2 , yVal2 }, 1);
		yVal1 = yVal2;
		xVal1 = xVal2;
	}


}



void RankGrapher::onGameEntry() {


	if (gameWrapper->IsInOnlineGame()) {
		MMRWrapper mw = gameWrapper->GetMMRWrapper();
		playlistNum = mw.GetCurrentPlaylist();

	}

	MMRWrapper mw = gameWrapper->GetMMRWrapper();

	uniqueID = gameWrapper->GetUniqueID();

	userPlaylist = mw.GetCurrentPlaylist();




	if (userPlaylist != 0) {

		gotNewMMR = false;
		while (!gotNewMMR) {

			if (gameWrapper->GetMMRWrapper().IsSynced(uniqueID, userPlaylist) && !gameWrapper->GetMMRWrapper().IsSyncing(uniqueID)) {

				// Makes sure it is one of the ranked gamemodes to prevent crashes
				if (!(find(begin(rankedPlaylists), end(rankedPlaylists), userPlaylist) != end(rankedPlaylists))) {
					return;
				}

				// Getting the mmr
				userMMR = gameWrapper->GetMMRWrapper().GetPlayerMMR(uniqueID, userPlaylist);
				gotNewMMR = true;
			}
		}


		currentMatchGuid = gameWrapper->GetOnlineGame().GetMatchGUID();





		fileName = to_string(userPlaylist) + "data.csv";
		fileNameTemp = to_string(userPlaylist) + "data_temp.csv";





		ofstream outputFile;
		ifstream inputFile;

		inputFile.open(gameWrapper->GetDataFolder() / "RankGrapher" / fileName);
		string line = "";


		//checking if data file exists, if not then creates file with first row, otherwise add data to file

		if (inputFile.fail()) {
			outputFile.open(gameWrapper->GetDataFolder() / "RankGrapher" / fileName);

			outputFile << "1" << "," << currentMatchGuid << "," << to_string(userMMR) << "," << currentSession << "\n";
			outputFile.close();
			inputFile.close();

		}

		else {



			outputFile.open(gameWrapper->GetDataFolder() / "RankGrapher" / fileNameTemp);

			int gameNumber;
			gameNumber = 0;
			string gameId;
			float mmr;
			vector<string> row;
			string tempString;
			int gameSession;

			while (getline(inputFile, line)) {



				stringstream inputString(line);

				getline(inputString, tempString, ',');
				gameNumber = atoi(tempString.c_str());
				getline(inputString, gameId, ',');

				getline(inputString, tempString, ',');
				mmr = atof(tempString.c_str());


				getline(inputString, tempString, ',');
				gameSession = atoi(tempString.c_str());
				
				outputFile << to_string(gameNumber) << "," << gameId << "," << to_string(mmr) << "," << to_string(gameSession) << "\n";

				line = "";


			}

			if (userMMR != mmr) {
				outputFile << to_string(gameNumber + 1) << "," << currentMatchGuid << "," << to_string(userMMR) << "," << to_string(currentSession) << "\n";
			}
			
			outputFile.close();
			inputFile.close();



			remove(gameWrapper->GetDataFolder() / "RankGrapher" / fileName.c_str());


			// renaming the updated file with the existing file name
			rename(gameWrapper->GetDataFolder() / "RankGrapher" / fileNameTemp.c_str(), gameWrapper->GetDataFolder() / "RankGrapher" / fileName.c_str());





		}
	} 
}

void RankGrapher::gameEnd(std::string eventName) {
	
	isWindowOpen_ = true;
	xOffset = 0;
	yOffset = 0;


	isGameEnd = 1;
	

	gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {
		RenderCanvas(canvas);
	});



	shouldHideCursor = false;
	shouldRenderImGui = true;
	
}

void RankGrapher::scoreboardLoad(std::string eventName) {

	isWindowOpen_ = true;
	xOffset = -170 * ((float)screenSize.X / 1920);
	yOffset = 90 * ((float)screenSize.Y / 1080) ;
	gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {
		RenderCanvas(canvas);
	});
	
	
	 shouldHideCursor = false;
	shouldRenderImGui = true;

}
void RankGrapher::loadGraph() {

	gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {
		RenderCanvas(canvas);
		});
	
}
void RankGrapher::loadMenu(string eventName) {
    gameWrapper->UnregisterDrawables();
    shouldRenderImGui = false;
	shouldHideCursor = false;
}

void RankGrapher::CloseWindow(string eventName) {
    gameWrapper->UnregisterDrawables();
    shouldHideCursor = true;
	shouldRenderImGui = false;
	//isWindowOpen_ = false;
}
