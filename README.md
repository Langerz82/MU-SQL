# MuMySQL
MuOnline - MySQL - Project for migrating all old MSSQL over to MySQL.
<br/><br/>
Would anyone be interested in creating a github with the source from here and try to convert it into a Linux/MySQL Server? Its very possible and I am a decent programmer I just need help with the tedious jobs, such as converting the Stored Procedures over.
<br/><br/>
Actually we could make it a console application that supports both Windows/Linux and MySQL.
<br/><br/>
The Main reason I want to do this is to keep costs down in NOT using licensed software. The original developers could benefit too as they will be able to use the source for there more recent versions and still maintain there piece of cake.
<br/><br/>
Here is the Repository, If anyone wants to contribute you can either send your gists in diff format via the issues section, or create a Pull Request to add more.
<br/><br/>
UPDATE - Just had a great idea. With the code being optimized as it is there will be no need for a DataServer application. Instead the Connect Server and Game Server will directly interact with the Database. This means that packets will no longer need to be marshalled across to the DataServer. The downside is that the the DataServer will no longer be able to be on a seperate Server but for the large majority this will not be even needed. Cool.
<br/><br/>
THE TODO LIST<br/><br/>
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

