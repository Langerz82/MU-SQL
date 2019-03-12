# MUSQL
<br/><br/>

# Overview
This project aims at creating a Top-Down Server Framework for Role-Playing Games. We aim at making the Server software cross-compatible for Windows/Linux and future Macs. To handle the Save state persistance currently only MySQL is supported. We aim at creating a generic platform that any kind of client rpg can be used and make use of the feature set of the Server. Currently we are trying to make it compatible with a well known commercial MU client. However the project in future phases each  each Server Feature will hopefully be de-coupled so you can enable/disable features with simple configuration file changes.
<br/><br/>

# Help Wanted - Programming
We currently are in desperate need of C++ Programmers with about a years programming experience of more to help remove the compile errors due to function signature changes. If you fairly new to C++, and know the differences between a referenced variable and a pointer and can code some of it using STL Libraries then we would appreciate your help. If you intend to help the ConnectServer is pretty much completed, but the GameServer itself needs much work done. If you intend to help please declare the file you are working on getting to compile ok so there is no overlapped work.<br/>
For the classes that do not compile function signatures should be preserved if they pass a GameObject reference or pointer. References are much preferred, to pass across functions, and pointers are better suited to obtaining the GameObject within the global function with the call getGameObject(index).<br/>
There are old code checks that check if the player has a valid index remove them as we will be only checking the getGameObject() for a nullptr and aborting that function gracefully should it occur. The player also does not need a check that they are online because the gGameObjects Map only has an entry inserted should there be an online player.<br/>
If it's a GameObject reference it should be Obj, ObjTarget, ObjMonster etc. If it's a pointer they should start with an lp, for example lpObj, lpObjTarget etc.<br/>
There are some parts of the code that iterate through an index array. They need to be converted to use the GameObject Map structure which can be done doing this: for each (std::pair<int, CGameObject*> user in gGameObjects) { // do something. }<br/>
I don't want to ever see anyone use a copied CGameObject for any purpose whatsoever. The idea is we directly access the class so all the variables do not need to be copied again and again (there is allot of variables). Remember if its part of a function signature it should reference, if its inside a function use a pointer and getGameObject(). The Only exception would probably be the GameProtocol which uses pointers in the function signatures which was done to save time. Technically this was a mistake which should be fixed in future.<br/>



# THE TODO LIST<br/><br/>
<pre>
DATABASE SIDE:
    MySQL - Create Table Structures - done.
        I have created the full DB Structures.
    MySQL - Add Stored Procedures by modifying MSSQL ones. - in progress.
        We are currently working on changing all the Stored Procedures over. 
        Don't bother migrating the SP's that have only one DELETE/INSERT/SELECT. 
        We can do that in C++ with query statements.
    Code - Integrate MySQL Support to ConnectServer/GameServer with existing code from WoW Mangos.
    Code - Adjust Query Calls from MSSQL to MySQL.
    Code - Adjust Stored Procedure Calls from MSSQL to MySQL.
<br/><br/>
WINDOWS TO LINUX/WIN CONSOLE APP:
    Code - CS/GS - Port MFC Windows Application to a OS Independent Console Application. - Nearly Completed.
    Code - CS/GS - Create commands to Reload Server Settings.
</pre>
<br/><br/>
refs:<br/>
http://forum.ragezone.com/f197/release-igcn-muemu-server-season-1146663/<br/>
http://forum.ragezone.com/f508/development-convert-ep12-windows-mssql-1158414/<br/>

