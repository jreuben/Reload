/*
Copyright (C) 2011 MoSync AB

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License,
version 2, as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA.
*/

/**
 * @file ReloadClient.h
 *
 *	Author: Ali Sarrafi, Iraklis Rossis, Mikael Kindborg
 */

#ifndef MOSYNC_RELOAD_RELOADCLIENT_H
#define MOSYNC_RELOAD_RELOADCLIENT_H

#include <Wormhole/HybridMoblet.h>
#include <Wormhole/MessageProtocol.h>
#include <Wormhole/MessageStream.h>
#include <Wormhole/Libs/JSONMessage.h>
#include <Wormhole/Libs/PhoneGap/PhoneGapMessageHandler.h>
#include <Wormhole/Libs/JSNativeUI/NativeUIMessageHandler.h>
#include <Wormhole/Libs/JSNativeUI/ResourceMessageHandler.h>
#include <NativeUI/Widgets.h>
#include <wchar.h>
#include <MAFS/File.h> // Library for file system bundles
#include <yajl/YAJLDom.h>


#include "ReloadScreenController.h"
#include "LoadingScreen.h"
#include "SocketHandler.h"
#include "DownloadHandler.h"
#include "MAHeaders.h"
#include "dataTypes.h"

// Forward declarations.
class ReloadScreenController;
class LoadingScreen;

/**
 * The ReloadClient application class.
 */
class ReloadClient :
	public Wormhole::HybridMoblet,
	public SocketHandlerListener,
	public DownloadHandlerListener,
	public Wormhole::LogMessageListener,
	public TimerListener
{
public:
	// ========== Creation and destruction ==========

	/**
	 * Constructor.
	 */
	ReloadClient();

	/**
	 * Destructor.
	 */
	virtual ~ReloadClient();

	void ReloadClient::runTimerEvent();

	/**
	 * Creation/initialization.
	 */
	void setScreenOrientation();
	void initializeWebView();
	void initializeVariables();
	void initializeFiles();
	void createScreens();
	void createMessageHandlers();
	void createNetworkHandlers();

	// ========== Implemented (inherited) methods  ==========

	/**
	 * Called from JavaScript when a Wormhole app has been loaded.
	 */
	virtual void openWormhole(MAHandle webViewHandle);

	/**
	 * This method is called when a key is pressed.
	 * Forwards the event to PhoneGapMessageHandler.
	 */
	virtual void keyPressEvent(int keyCode, int nativeCode);

	/**
	 * We want to quit the ReloadClient only if an app is not running.
	 * This method is called from the WOrmhole library when a JavaScript
	 * application requests to exit.
	 */
	virtual void exit();

	/**
	 * Method in interface LogMessageListener.
	 */
	void onLogMessage(const char* message, const char* url);

	// ========== SocketHandlerListener methods  ==========
	/**
	 * A connection to the server has been established.
	 */
	void socketHandlerConnected(int result);

	/**
	 * We received a message from the server.
	 */
	void socketHandlerDisconnected(int result);

	/**
	 * We received a message from the server.
	 */
	void socketHandlerMessageReceived(const char* message);

	// ========== DownloadHandlerListener methods  ==========

	/**
	 * Called on downloaded error.
	 */
	void downloadHandlerError(int code);

	/**
	 * Called on download success. Note that we must destroy
	 * the data object.
	 */
	void downloadHandlerSuccess(MAHandle data);

	// ========== Methods called from the UI  ==========

	/**
	 * Called from the UI to cancel the download.
	 */
	void cancelDownload();

	/**
	 * Open connection to the Reload server.
	 * @param serverAddress IP-address as a string.
	 */
	void connectToServer(const char* serverAddress);

	/**
	 * Disconnect from the Reload server.
	 */
	void disconnectFromServer();

	/**
	 * Shows an alert with the disconnection message from server
	 * @param disconnectData The message that the server sent
	 *						 when disconnecting the client
	 */
	void showDisconnectionMessage (MAUtil::String disconnectData);

	// ========== Server message handling  ==========

	/**
	 * Handle JSON messages.
	 */
	void handleJSONMessage(const MAUtil::String& json);

	// ========== Download methods ==========

	/**
	 * Download an app bundle from the server.
	 */
	void downloadBundle(const String& urlData, int fileSize);

	void reloadBundle(const String& json);
	/**
	 * Experimental method not used.
	 */
	// void downloadHTML();

	// ========== Evaluate JavaScript ==========

	/**
	 * Evaluate the given script in the main web view and
	 * send the result back using mosync.rlog().
	 * @param script
	 */
	void evaluateScript(const String& script);

	/**
	 * Evaluate the given script in the main web view and
	 * send the result back using mosync.bridge.send(['Custom',
	 * 'EvalResponse', {'res': response, 'fingerprint': id}])
	 * @param script
	 * @param fingerprint
	 */
	void evaluate(const String& script, const String& fingerprint);

	 /**
	 * Send result of script evaluation to the server.
	 * @param message
	 */
	void evalResponse(Wormhole::MessageStream& message);

	// ========== Launching apps ==========

	/**
	 * Loads the HTML files that were extracted last time.
	 */
	void launchSavedApp(MAUtil::String projectName);

	/**
	 * Resets the client (destroys widgets and stops sensors)
	 * in preparation for a new app
	 */
	void freeHardware();

	/**
	 * Empty the folder where apps are stored.
	 */
	void clearAppsFolder(MAUtil::String appFolder);

	// ========== Send info to server  ==========
	/**
	 * Send a message requesting project list
	 */
	void getProjectListFromServer();

	/**
	 * Send a message requesting a project so it can be saved
	 * @param projectName The name of the project to be saved
	 */
	void saveProjectFromServer(MAUtil::String projectName);

	/**
	 * Send a message requesting a project to be reloaded
	 */
	void reloadProjectFromServer(MAUtil::String projectName);

	/**
	 * Sends information about the device to the server.
	 */
	void sendClientDeviceInfo();

	// ========== Helper methods ==========

	/**
	 * Send a TCP message to the server. Prepends the message
	 * with the message length as an 8 char hex string.
	 * @param message Message to send.
	 */
	void sendTCPMessage(const MAUtil::String& message);

	/**
	 * Get client info.
	 * @return String with client info.
	 */
	MAUtil::String getInfo();

	/**
	 * This method handles any connection error messages.
	 * @param errorCode The error code that was returned.
	 */
	void showConnectionErrorMessage(int errorCode);

	/**
	 * Getter returns the vector of projects on server
	 * @return MAUtil::Vector <reloadProject>
	 */
	MAUtil::Vector <reloadProject> * getListOfProjects();

	/**
	 * Getter returns the vector of projects stored on the device
	 * @return MAUtil::Vector <reloadProject>
	 */
	MAUtil::Vector <reloadProject> * getListOfSavedProjects();

	/**
	 * Getter: Returns the servers last used ip address
	 * @return A string with the ip address
	 */
	MAUtil::String getServerIpAddress();

private:
	/**
	 * Class that handles the Login Screen UI.
	 */
	ReloadScreenController* mReloadScreenController;

	/**
	 * Class that handles the Loading screen UI.
	 */
	LoadingScreen* mLoadingScreen;

	/**
	 * true when an app is running, false if on the login screen.
	 */
	bool mRunningApp;

	/**
	 * TODO: Document what this variable is used for.
	 */
	bool mHasPage;

	/**
	 * Flag to signal running of tests.
	 */
	bool mRunTests;

	/**
	 * The platform the client is running on.
	 */
	MAUtil::String mOS;

	/**
	 * The client information (version, timestamp).
	 */
	MAUtil::String mInfo;

	/**
	 * The address of the server we are connected to.
	 */
	MAUtil::String mServerAddress;

	/**
	 * The general folder where app files reside.
	 */
	MAUtil::String mAppsFolder;

	/**
	 * The general folder where saved apps reside.
	 */
	MAUtil::String mSavedAppsFolder;

	/**
	 * The relative path to the downloaded app folder.
	 */
	MAUtil::String mAppPath;

	/**
	 * Expected size of the downloaded bundle.
	 */
	int mBundleSize;

	/**
	 * Object that handles socket connections.
	 */
	SocketHandler mSocketHandler;

	/**
	 * Object that handles downloads.
	 */
	DownloadHandler mDownloadHandler;

	/**
	 * Clients Protocol Version
	 */
	char* mProtocolVersion;

	/**
	 * Vector of type reloadProject that stores data about the projects
	 * that are on the server
	 */
	MAUtil::Vector <struct reloadProject> mProjects;

	/**
	 * Vector that stores the saved projects that are located on the
	 * device for offline use
	 */
	MAUtil::Vector <struct reloadProject> mSavedProjects;

	/**
	 * If it set then the app will be saved. I not normal reload functionality.
	 */
	MAUtil::String mProjectToSave;
};

#endif
