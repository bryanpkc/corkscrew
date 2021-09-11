// A Go alternative to the Legacy Corkscrew written in C
// This should be way more portable and compatible going forward.
package main

import (
	"bytes"
	"encoding/base64"
	"fmt"
	"io"
	"io/ioutil"
	"net"
	"os"
	"os/signal"
	"syscall"
)

type cParams struct {
	proxyHost string
	proxyPort uint16
	destHost  string
	destPort  uint16
	authFile  string
}

func main() {
	CorkscrewCmd.Execute()
}

func proxy(p *cParams) {
	// Spin up the signal handler goroutine
	waitForTerm, fn := getSignalHandler()
	go fn()
	conn := connect(p)
	proxyConnectRequest := buildConnectRequest(p)
	conn.Write(proxyConnectRequest.Bytes())
	go io.Copy(conn, os.Stdin)
	go io.Copy(os.Stdout, conn)
	defer conn.Close()
	<-waitForTerm
}

func getAuthParams(p *cParams) string {
	rv := os.Getenv("CORKSCREW_AUTH")
	if p.authFile != "" {
		authParamsBytes, err := ioutil.ReadFile(p.authFile)
		if err != nil {
			fmt.Printf("Error reading file %s: %s\n", p.authFile, err.Error())
			os.Exit(2)
		}
		rv = bytes.NewBuffer(authParamsBytes).String()
	}
	return rv
}

func connect(p *cParams) *net.TCPConn {
	tcpAddr, err := net.ResolveTCPAddr("tcp4", fmt.Sprintf("%s:%d", p.proxyHost, p.proxyPort))
	if err != nil {
		fmt.Printf("unable to resolve address %s:%d (%s)\n", p.proxyHost, p.proxyPort, err.Error())
		os.Exit(3)
	}
	conn, err := net.DialTCP("tcp", nil, tcpAddr)
	if err != nil {
		fmt.Printf("connect failure: %s\n", err.Error())
		os.Exit(3)
	}
	return conn
}

func buildConnectRequest(p *cParams) bytes.Buffer {
	// Build the CONNECT string to use to connect through the HTTP proxy
	op := bytes.Buffer{}
	op.WriteString("CONNECT ")
	op.WriteString(fmt.Sprintf("%s:%d", p.destHost, p.destPort))
	op.WriteString(" HTTP/1.0")
	authParams := getAuthParams(p)
	if authParams != "" {
		op.WriteString("\nProxy-Authorization: Basic ")
		var auth bytes.Buffer
		base64.NewEncoder(base64.StdEncoding, &auth).Write(bytes.NewBufferString(authParams).Bytes())
		op.Write(auth.Bytes())
	}
	op.WriteString("\n\n")
	return op
}

func getSignalHandler() (<-chan bool, func()) {
	done := make(chan bool, 1)
	sigs := make(chan os.Signal, 1)

	signal.Notify(sigs, syscall.SIGINT, syscall.SIGTERM, syscall.SIGPIPE)

	return done, func() {
		sig := <-sigs
		fmt.Println("Received signal ")
		fmt.Println(sig)
		os.Exit(-1)
		done <- true
	}
}
