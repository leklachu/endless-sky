/* Preferences.cpp
Copyright (c) 2014 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "Preferences.h"

#include "Audio.h"
#include "DataFile.h"
#include "DataNode.h"
#include "DataWriter.h"
#include "Files.h"
#include "Screen.h"

#include <algorithm>
#include <map>

using namespace std;

namespace {
	map<string, bool> settings;
	int scrollSpeed = 60;
	
	// Strings for ammo expenditure:
	static const string EXPEND_AMMO = "Escorts expend ammo";
	static const string FRUGAL_ESCORTS = "Escorts use ammo frugally";
	
	static const vector<double> ZOOMS = {.25, .35, .50, .70, 1.00, 1.40, 2.00};
	int zoomIndex = 4;
}



void Preferences::Load()
{
	settings["Automatic aiming"] = true;
	settings["Render motion blur"] = true;
	settings["Escorts use ammo frugally"] = true;
	settings["Escorts expend ammo"] = true;
	settings["Warning siren"] = true;
	settings["Show mini-map"] = true;
	settings["Show planet labels"] = true;
	settings["Show hyperspace flash"] = true;
	settings["Draw background haze"] = true;
	settings["Reduce large graphics"] = false;
	settings["Hide unexplored map regions"] = true;
	
	DataFile prefs(Files::Config() + "preferences.txt");
	for(const DataNode &node : prefs)
	{
		if(node.Token(0) == "window size" && node.Size() >= 3)
			Screen::SetRaw(node.Value(1), node.Value(2));
		else if(node.Token(0) == "zoom" && node.Size() >= 2)
			Screen::SetZoom(node.Value(1));
		else if(node.Token(0) == "volume" && node.Size() >= 2)
			Audio::SetVolume(node.Value(1));
		else if(node.Token(0) == "scroll speed" && node.Size() >= 2)
			scrollSpeed = node.Value(1);
		else if(node.Token(0) == "view zoom")
			zoomIndex = node.Value(1);
		else
			settings[node.Token(0)] = (node.Size() == 1 || node.Value(1));
	}
}



void Preferences::Save()
{
	DataWriter out(Files::Config() + "preferences.txt");
	
	out.Write("volume", Audio::Volume());
	out.Write("window size", Screen::RawWidth(), Screen::RawHeight());
	out.Write("zoom", Screen::Zoom());
	out.Write("scroll speed", scrollSpeed);
	out.Write("view zoom", zoomIndex);
	
	for(const auto &it : settings)
		out.Write(it.first, it.second);
}



bool Preferences::Has(const string &name)
{
	auto it = settings.find(name);
	return (it != settings.end() && it->second);
}



void Preferences::Set(const string &name, bool on)
{
	settings[name] = on;
}



void Preferences::ToggleAmmoUsage()
{
	bool expend = Has(EXPEND_AMMO);
	bool frugal = Has(FRUGAL_ESCORTS);
	Preferences::Set(EXPEND_AMMO, !(expend && !frugal));
	Preferences::Set(FRUGAL_ESCORTS, !expend);
}



string Preferences::AmmoUsage()
{
	return Has(EXPEND_AMMO) ? Has(FRUGAL_ESCORTS) ? "frugally" : "always" : "never";
}

// Scroll speed preference.
int Preferences::ScrollSpeed()
{
	return scrollSpeed;
}



void Preferences::SetScrollSpeed(int speed)
{
	scrollSpeed = speed;
}



// View zoom.
double Preferences::ViewZoom()
{
	return ZOOMS[zoomIndex];
}



void Preferences::ZoomViewIn()
{
	zoomIndex = min(static_cast<int>(ZOOMS.size() - 1), zoomIndex + 1);
}



void Preferences::ZoomViewOut()
{
	zoomIndex = max(0, zoomIndex - 1);
}
