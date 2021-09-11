// Command line argument parsing and sanity checking

package main

import (
	"fmt"
	"os"
	"strconv"

	"github.com/spf13/cobra"
)

func usage() {
	fmt.Println("corkscrew Go 0.01 (gkalele derived from agroman@agroman.net corkscrew)")
	fmt.Println("usage: corkscrew <proxyhost> <proxyport> <desthost> <destport> [authfile]")
}

var CorkscrewCmd = &cobra.Command{
	Use:   "corkscrew",
	Short: "An SSH over HTTP proxy",
	Long: `Corkscrew is a tool for tunneling SSH through HTTP proxies, but... you
might find another use for it.`,
	Run: func(cmd *cobra.Command, args []string) {
		if len(args) < 4 {
			fmt.Printf("Insufficient number of args (5) in %+v\n", args)
			usage()
			os.Exit(1)
		}
		params := cParams{
			proxyHost: fetchStringArg(args, 0),
			proxyPort: fetchUint16Arg(args, 1),
			destHost:  fetchStringArg(args, 2),
			destPort:  fetchUint16Arg(args, 3),
			authFile:  fetchStringArgOpt(args, 4),
		}
		proxy(&params)
	},
}

func fetchStringArg(args []string, n int) string {
	if (n + 1) <= len(args) {
		return args[n]
	}
	usage()
	os.Exit(1)
	return ""
}

func fetchUint16Arg(args []string, n int) uint16 {
	a := fetchStringArg(args, n)
	num, err := strconv.ParseUint(a, 10, 16)
	if err != nil {
		usage()
		os.Exit(1)
	}
	return uint16(num)
}

func fetchStringArgOpt(args []string, n int) string {
	if (n + 1) <= len(args) {
		return args[n]
	}
	return ""
}
