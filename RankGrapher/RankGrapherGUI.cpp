#include "pch.h"
#include "RankGrapher.h"



std::string RankGrapher::GetPluginName() {
	return "RankGrapher plugin";
}


// Render the plugin settings here
// This will show up in bakkesmod when the plugin is loaded at
//  f2 -> plugins -> RankGrapher
void RankGrapher::RenderSettings() {
	ImGui::TextUnformatted("RankGrapher plugin settings");
	static bool check1 = cvarManager->getCvar("displayOnScoreboard").getBoolValue();
	static bool check2 = cvarManager->getCvar("displayAfterGame").getBoolValue();
	static bool check3 = cvarManager->getCvar("enableSmoothness").getBoolValue();

	ImGui::Checkbox("Display overlay under scoreboard", &check1);

	CVarWrapper displayOnScoreboardCVar = cvarManager->getCvar("displayOnScoreboard");
	if (displayOnScoreboardCVar) {
		if (check1) {
			displayOnScoreboardCVar.setValue(1);
			gameWrapper->HookEvent("Function TAGame.GFxData_GameEvent_TA.OnOpenScoreboard", bind(&RankGrapher::scoreboardLoad, this, std::placeholders::_1));
			gameWrapper->HookEvent("Function TAGame.GFxData_GameEvent_TA.OnCloseScoreboard", bind(&RankGrapher::loadMenu, this, std::placeholders::_1));
		}
		else {
			displayOnScoreboardCVar.setValue(0);
			gameWrapper->UnhookEvent("Function TAGame.GFxData_GameEvent_TA.OnOpenScoreboard");
			gameWrapper->UnhookEvent("Function TAGame.GFxData_GameEvent_TA.OnCloseScoreboard");
		}
	}

	ImGui::Checkbox("Display overlay after game", &check2);

	CVarWrapper displayAfterGameCVar = cvarManager->getCvar("displayAfterGame");
	if (displayAfterGameCVar) {
		if (check2) {
			displayAfterGameCVar.setValue(1);
			gameWrapper->HookEvent("Function GameEvent_Soccar_TA.PodiumSpotlight.EndState", bind(&RankGrapher::gameEnd, this, std::placeholders::_1));
		}
		else {
			displayAfterGameCVar.setValue(0);
			gameWrapper->UnhookEvent("Function GameEvent_Soccar_TA.PodiumSpotlight.EndState");
		}
	}

	ImGui::Checkbox("Enable custom smoothness", &check3);

	CVarWrapper enableSmoothnessCVar = cvarManager->getCvar("enableSmoothness");
	if (enableSmoothnessCVar) {
		if (check3) {
			enableSmoothnessCVar.setValue(1);
		}
		else {
			enableSmoothnessCVar.setValue(0);
		}
	}

	static int smoothnessInt = cvarManager->getCvar("smoothness").getBoolValue();
	ImGui::InputInt("smoothness value", &smoothnessInt);
	if (smoothnessInt > 0) {
		CVarWrapper smoothnessCVar = cvarManager->getCvar("smoothness");
		if (smoothnessCVar) {
			smoothnessCVar.setValue(smoothnessInt);
		}
	}

	ImGui::Text("Change line color");
	ImGui::PushItemWidth(200);
	ImGui::ColorPicker4("MyColor##4", (float*)&color, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
	ImGui::PopItemWidth();
    
    CVarWrapper scoreboardXCVar = cvarManager->getCvar("scoreboardSettingsX");
    CVarWrapper scoreboardYCVar = cvarManager->getCvar("scoreboardSettingsY");
    CVarWrapper xCVar = cvarManager->getCvar("gameEndSettingsX");
    CVarWrapper yCVar = cvarManager->getCvar("gameEndSettingsY");

    ImGui::TextUnformatted("Position offset");

    if (scoreboardXCVar && scoreboardYCVar && xCVar && yCVar) {
        int scoreboardX = scoreboardXCVar.getIntValue();
        int scoreboardY = scoreboardYCVar.getIntValue();
        int x = xCVar.getIntValue();
        int y = yCVar.getIntValue();

        ImGui::DragInt("X offset under scoreboard", &scoreboardX);
        ImGui::DragInt("Y offset under scoreboard", &scoreboardY);
        ImGui::DragInt("X offset after game", &x);
        ImGui::DragInt("Y offset after game", &y);
        scoreboardXCVar.setValue(scoreboardX);
        scoreboardYCVar.setValue(scoreboardY);
        xCVar.setValue(x);
        yCVar.setValue(y);
    }
    
}



// Do ImGui rendering here
void RankGrapher::Render() {


	
	
	//gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {
	//	RenderCanvas(canvas);
	//	});
	
	ofstream outputFile2;
    
    
	ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar;
	ImGui::SetNextWindowPos(ImVec2(800 * ((float)screenSize.X / 1920) + xOffset, 645 * ((float)screenSize.Y / 1080) + yOffset));

	//ImGui::SetNextWindowSize(ImVec2(650, 680));
  
	if (!ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_, WindowFlags))
	{
		// Early out if the window is collapsed, as an optimization.

		ImGui::End();
		return;
	}
    LOG("{}", shouldRenderImGui);
    if (shouldRenderImGui) {


        CVarWrapper timeframeCVar = cvarManager->getCvar("timeframe");
        if (!timeframeCVar) {
            ImGui::End();
            //LOG("{}", timeframeCVar.getStringValue());
            return;
        }

        //screenSize = gameWrapper->GetScreenSize();
        string timeframe = timeframeCVar.getStringValue();

        ImGuiStyle* style = &ImGui::GetStyle();
        ImVec4* colors = style->Colors;

        //1.00f, 1.00f, 1.00f, 1.00f
        colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.40f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.50f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.50f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(.50f, 0.50f, 0.00f, 0.50f);





        auto gui = gameWrapper->GetGUIManager();
        ImFont* fontBig = gui.GetFont("PantonBig");

        ImDrawList* drawList = ImGui::GetWindowDrawList();


        //int playlistNum = 13;




        //screenSize = gameWrapper->GetScreenSize();
        ImColor white = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        ImGui::PushFont(fontBig);
        float pos1 = 805 * ((float)screenSize.X / 1920);
        float pos2 = 652 * ((float)screenSize.Y / 1080);
        drawList->AddText(fontBig, 25, ImVec2(1180 * ((float)screenSize.X / 1920) + xOffset, 658 * ((float)screenSize.Y / 1080) + yOffset), ImU32(white), "Timeframe");
        //LOG("{}", playlistNum);
        if (playlistNum == 10) {
            drawList->AddText(fontBig, 30, ImVec2(pos1 + xOffset, pos2 + yOffset), ImU32(white), "1v1 Rank Progression");
        }
        if (playlistNum == 11) {
            drawList->AddText(fontBig, 30, ImVec2(pos1 + xOffset, pos2 + yOffset), ImU32(white), "2v2 Rank Progression");
        }
        if (playlistNum == 13) {
            drawList->AddText(fontBig, 30, ImVec2(pos1 + xOffset, pos2 + yOffset), ImU32(white), "3v3 Rank Progression");
        }
        if (playlistNum == 27) {
            drawList->AddText(fontBig, 30, ImVec2(pos1 + xOffset, pos2 + yOffset), ImU32(white), "Rumble Rank Progression");
        }
        if (playlistNum == 28) {
            drawList->AddText(fontBig, 30, ImVec2(pos1 + xOffset, pos2 + yOffset), ImU32(white), "Rumble Rank Progression");
        }
        if (playlistNum == 29) {
            drawList->AddText(fontBig, 30, ImVec2(pos1 + xOffset, pos2 + yOffset), ImU32(white), "Dropshot Rank Progression");
        }
        if (playlistNum == 30) {
            drawList->AddText(fontBig, 30, ImVec2(pos1 + xOffset, pos2 + yOffset), ImU32(white), "Snow Day Rank Progression");
        }
        if (playlistNum == 34) {
            drawList->AddText(fontBig, 30, ImVec2(pos1 + xOffset, pos2 + yOffset), ImU32(white), "Tournament Rank Progression");
        }

        ImGui::PopFont();

        ImGui::SameLine();

        ImGui::PushItemWidth(70);

        ImGui::SetCursorPos(ImVec2(495, 15));



        static int item_current_idx = 0;
        //LOG("{}", timeframe);
        if (timeframe == "all") {
            static int item_current_idx = 0;
        }
        if (timeframe == "session") {
            static int item_current_idx = 1;
        }
        const char* items[] = { "All", "Session" };
        // Here we store our selection data as an index.
        const char* combo_preview_value = items[item_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)

        if (ImGui::BeginCombo("", combo_preview_value))
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                const bool is_selected = (item_current_idx == n);
                if (ImGui::Selectable(items[n], is_selected)) {
                    item_current_idx = n;
                    if (item_current_idx == 0) {
                        timeframeCVar.setValue("all");
                    }
                    if (item_current_idx == 1) {
                        timeframeCVar.setValue("session");
                    }

                }


                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();

            }



            ImGui::EndCombo();


        }
        ImGui::PopItemWidth();
        //ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(570, 13));
        //LOG("loaded");

        colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 1.0f, 0.98f, 0.40f);
        colors[ImGuiCol_ButtonActive] = ImVec4(1.0f, 1.f, 0.98f, 0.40f);

        if (closeImg->IsLoadedForImGui()) {

            ImTextureID my_tex_id = closeImg->GetImGuiTex();
            auto closeImgRect = closeImg->GetSizeF();

            ImVec2 size = ImVec2(71.0f, 22.0f);                     // Size of the image we want to make visible
            ImVec2 uv0 = ImVec2(0.0f, 0.0f);                        // UV coordinates for lower-left
            ImVec2 uv1 = ImVec2(1.0f, 1.0f);// UV coordinates for (32,32) in our texture
            ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);         // No background
            ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);       // No tint

            if (ImGui::ImageButton(my_tex_id, size, uv0, uv1, 1, bg_col, tint_col)) {
                shouldRenderImGui = false;

                isWindowOpen_ = false;

                gameWrapper->UnregisterDrawables();
            }


        }


        colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.0f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    }
        
	if (shouldHideCursor) {
		ImGui::SetMouseCursor(ImGuiMouseCursor_None);
	}

	ImGui::End();

	
	
}
void RankGrapher::RenderImGui() {

}
// Name of the menu that is used to toggle the window.
std::string RankGrapher::GetMenuName()
{
	return "RankGrapher";
}

// Title to give the menu
std::string RankGrapher::GetMenuTitle()
{
	return menuTitle_;
}

// Don't call this yourself, BM will call this function with a pointer to the current ImGui context
void RankGrapher::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
  
}

// Should events such as mouse clicks/key inputs be blocked so they won't reach the game
bool RankGrapher::ShouldBlockInput()
{
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

// Return true if window should be interactive
bool RankGrapher::IsActiveOverlay()
{
	return true;
}

// Called when window is opened
void RankGrapher::OnOpen()
{
	isWindowOpen_ = true;
    
   
}

// Called when window is closed
void RankGrapher::OnClose()
{
	gameWrapper->UnregisterDrawables();
	isWindowOpen_ = false;
    shouldRenderImGui = false;
}

