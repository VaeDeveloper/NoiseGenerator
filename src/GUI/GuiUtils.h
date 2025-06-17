#pragma once 


#define SHOW_HIDDEN_TAB_BAR(dock_id) \
    do { \
        if ((dock_id) != 0) { \
            if (ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id)) { \
                if (node->WantHiddenTabBarUpdate) node->WantHiddenTabBarToggle = true; \
            } \
        } \
    } while (0)




