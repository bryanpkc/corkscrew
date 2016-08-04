Welcome to Corkscrew
--------------------

Introduction
------------
Corkscrew is a tool for tunneling SSH through HTTP proxies, but
...you might find another use for it.

Corkscrew has been compiled on :
 * HPUX 
 * Solaris
 * FreeBSD
 * OpenBSD
 * Linux
 * Win32 (with Cygwin)

Corkscrew has been tested with the following HTTP proxies :
 * Gauntlet
 * CacheFlow
 * JunkBuster
 * Apache mod_proxy

Please email me if you get it working on other proxies or compile
it elsewhere.


Where Do I Get It?
------------------
Corkscrew's primary distribution site is :
http://www.agroman.net/corkscrew/ 


How Do I Build It?
------------------
In the corkscrew directory type './configure' then 'make'.  Check
out the INSTALL file for more information.


How Do I Install It?
--------------------
In the corkscrew directory type 'make install'.


How Is It Used?
---------------
Setting up Corkscrew with SSH/OpenSSH is very simple.  Adding
the following line to your ~/.ssh/config file will usually do
the trick (replace proxy.example.com and 8080 with correct values):

ProxyCommand /usr/local/bin/corkscrew proxy.example.com 8080 %h %p

NOTE: Command line syntax has changed since version 1.5.  Please
notice that the proxy port is NOT optional anymore and is required
in the command line.


How Do I Use The HTTP Authentication Feature?
---------------------------------------------
You will need to create a file that contains your usename and password
in the form of :
username:password

I suggest you place this file in your ~/.ssh directory.

After creating this file you will need to ensure that the proper perms
are set so nobody else can get your username and password by reading
this file.  So do this :
chmod 600 myauth

Now you will have to change the ProxyCommand line in your ~/.ssh/config
file.  Here's an example :

ProxyCommand /usr/local/bin/corkscrew proxy.work.com 80 %h %p ~/.ssh/myauth

The proxy authentication feature is very new and has not been tested
extensively so your mileage may vary.  If you encounter any problems
when trying to use this feature please email me.  It would be helpful
if you could include the following information :
- Proxy version (ie. Gauntlet Proxy, Microsoft Proxy Server, etc)
- Operating system you are trying to run corkscrew on
- Command line syntax you are using
- Any error messages that are visible to you

*NOTE: I have had problems using the auth features with Mircosoft Proxy
 server.  The problems are sporadic, and I believe that they are related
 to the round-robin setup that I was testing it again.  Your mileage may
 vary.


Who Am I?
---------
My name is Pat Padgett.  I'm a dork.
URL   : http://www.agroman.net/
Email : agroman@agroman.net

This has been a traumatized production.  http://www.trauma-inc.com/
