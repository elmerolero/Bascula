package main

import (
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
	"os/exec"
	"strings"
)

// P
var programaRuta = "https://miniplus.ddns.net/programas/scb/"

//var programaRuta = "http://localhost/programas/SCB/"

// Descarga un archivo
func DownloadFile(filepath string, url string) error {
	fmt.Println(url)
	// Get the data
	resp, err := http.Get(url)
	if err != nil {
		return err
	}
	defer resp.Body.Close()

	// Create the file
	out, err := os.Create(filepath)
	if err != nil {
		return err
	}
	defer out.Close()

	// Write the body to file
	_, err = io.Copy(out, resp.Body)
	return err
}

// Retorna si un archivo ezxiste
func existe(path string) (bool, error) {
	_, err := os.Stat(path)
	if err == nil {
		return true, nil
	}
	if os.IsNotExist(err) {
		return false, nil
	}

	return false, err
}

func main() {
	fmt.Println("Buscando actualizaciones.")
	getJSON(programaRuta)
	fmt.Println("Iniciando la aplicación.")
	comando := exec.Command("lanzador.exe")
	err := comando.Start()
	if err != nil {
		fmt.Errorf(err.Error())
	}
	comando.Process.Release()
	//var x map[string]interface{}
}

type archivo struct {
	Archivo string `json:"archivo"`
	Hash    string `json:"hash"`
}

func getJSON(url string) error {
	var result []archivo
	resp, err := http.Get(url + "elementos.json")
	if err != nil {
		return fmt.Errorf("cannot fetch URL %q: %v", url, err)
	}
	defer resp.Body.Close()
	if resp.StatusCode != http.StatusOK {
		return fmt.Errorf("unexpected http GET status: %s", resp.Status)
	}
	// We could check the resulting content type
	// here if desired.
	body, err := io.ReadAll(resp.Body)
	cuerpo := string(body)

	err = json.Unmarshal([]byte(cuerpo), &result)
	if err != nil {
		return fmt.Errorf("cannot decode JSON: %v", err)
	}

	for elemento := range result {
		var ruta string = programaRuta + result[elemento].Archivo[3:]
		ruta = strings.Replace(ruta, "\\", "/", -5)

		var posicion int
		for pos, char := range result[elemento].Archivo {
			if char == '\\' {
				posicion = pos
			}
		}

		// Obtiene el directorio
		directorio := result[elemento].Archivo[0:posicion]

		// Revisa si el directorio dado no existe para crearlo
		if _, err := os.Stat(directorio); os.IsNotExist(err) {
			// Crea el directorio
			os.MkdirAll(directorio, os.ModePerm)
		}

		// Verifica si el archivo existe
		ex, _ := existe(result[elemento].Archivo)
		if ex {
			hasher := sha256.New()
			f, err := os.Open(result[elemento].Archivo)
			if err != nil {
				log.Fatal(err)
			}
			defer f.Close()
			if _, err := io.Copy(hasher, f); err != nil {
				log.Fatal(err)
			}
			var hash string = hex.EncodeToString(hasher.Sum(nil))
			if hash != result[elemento].Hash {
				fmt.Println("Actualizando '" + result[elemento].Archivo + "'")
				DownloadFile(result[elemento].Archivo, ruta)
			}
		} else {
			fmt.Println("Descargando '" + result[elemento].Archivo + "'")
			DownloadFile(result[elemento].Archivo, ruta)
		}
	}
	return nil
}
