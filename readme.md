register and start server with:

`regsvr32 spool-ss.dll`

this create autostart RbmmRP Rpc Service
it create named pipe \Device\NamedPipe\spoolss (can be conflict with Print Spooler service)
and listen for clients ( spool-cc.exe )
3 operations supported:
download files from server to client
exec exe from client machine on server.
exe run from memory (without storing on disk) and under user credentials
by default current user (which exec spool-cc.exe ) used. but possible enter another credentials (mandatory in case user from another domain)