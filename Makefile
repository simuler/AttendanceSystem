all: server client client_ad

server:
	g++ util.cpp server.cpp -o server
client:
	g++ util.cpp client.cpp -o client
client_ad:
	g++ util.cpp client_administrators.cpp -o client_ad

clean:
	rm client server client_ad