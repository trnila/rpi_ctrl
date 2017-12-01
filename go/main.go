package main

// #include "erpc_client_setup.h"
// #include "messages.h"
// #cgo CFLAGS: -I/usr/local/include/erpc
// #cgo CXXFLAGS: -I/usr/local/include/erpc
// #cgo LDFLAGS: -lstdc++ -L/usr/local/lib/ -lerpc
import "C"

import (
	"net/http"

	"github.com/gorilla/mux"
	"log"
	"strconv"
	"github.com/rs/cors"
)

func erpc_init(path string, baud int) {
	var transport C.erpc_transport_t
	transport = C.erpc_transport_serial_init(C.CString(path), C.long(baud))

	var buffer C.erpc_mbf_t
	buffer = C.erpc_mbf_dynamic_init()

	C.erpc_client_init(transport, buffer)
}


func main() {
	erpc_init("/dev/ttyAMA0", 921600)

	r := mux.NewRouter()
	r.HandleFunc("/led/{led}/{state}", handle_led_state)

	handler := cors.Default().Handler(r)

	log.Fatal(http.ListenAndServe(":8080", handler))

}

func handle_led_state(writer http.ResponseWriter, request *http.Request) {
	vars := mux.Vars(request)

	led, err := strconv.ParseUint(vars["led"], 10, 8); if err != nil {
		http.Error(writer, "Invalid led value", http.StatusBadRequest)
		return
	}

	state, err := strconv.ParseBool(vars["state"]); if err != nil {
		http.Error(writer, "Invalid state", http.StatusBadRequest)
		return
	}


	C.set_led(1, C.uint8_t(1 << led), C._Bool(state))

	writer.Write([]byte("ok"))
}
