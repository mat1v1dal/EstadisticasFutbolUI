#ifndef EQUIPO_H
#define EQUIPO_H
#include <string>
#include <unordered_map>
#include "../lib/ArbolBinarioAVL.h"

class Partido;


struct Estadisticas {
    int golesAFavor = 0;
    int golesEnContra = 0;
    int partidosJugados = 0;
    int victorias = 0;
    int derrotas = 0;
};

class Equipo{
private:
    std::string nombre;
    int partidosJugados;
    int empates;
    int victorias;
    int derrotas;
    int golesAFavorTotales = 0;
    int golesEnContraTotales = 0;
    float promedioGoles;
    std::unordered_map<std::string, Estadisticas> estadisticasPorCompeticion;
public:
    Equipo() : nombre(""), victorias(0), derrotas(0), empates(0), partidosJugados(0), golesAFavorTotales(0), golesEnContraTotales(0), promedioGoles(0) {}
    Equipo(const std::string& nombre) : nombre(nombre), victorias(0), derrotas(0), empates(0), partidosJugados(0), golesAFavorTotales(0), golesEnContraTotales(0), promedioGoles(0) {}
    Equipo(const std::string& nombre, int partidosJugados, int empates, int victorias, int derrotas)
        : nombre(nombre),
        partidosJugados(partidosJugados),
        empates(empates),
        victorias(victorias),
        derrotas(derrotas) {}

    std::string getNombre() const{
        return nombre;
    };

    int getVictorias() const{
        return victorias;
    };
    int getDerrotas() const{
        return derrotas;
    };

    int getGolesAFavor() const{
        return golesAFavorTotales;
    };
    int getGolesEnContra() const{
        return golesEnContraTotales;
    };
    void registrarVictoria(const std::string& competicion) {
        estadisticasPorCompeticion[competicion].victorias += 1;
        estadisticasPorCompeticion[competicion].partidosJugados += 1;
    }

    void registrarDerrota(const std::string& competicion) {
        estadisticasPorCompeticion[competicion].derrotas += 1;
        estadisticasPorCompeticion[competicion].partidosJugados += 1;
    }

    void registrarEmpate(){
        empates++;
        partidosJugados++;
    };
    void aumentarGolesAFavorTotales(int cantidad){
        golesAFavorTotales += cantidad;
    };
    void aumentarGolesEnContraTotales(int cantidad){
        golesEnContraTotales += cantidad;
    };
    void aumentarGolesAFavor(const std::string& competicion, int cantidad) {
        estadisticasPorCompeticion[competicion].golesAFavor += cantidad;
    }
    void aumentarPartidosJugados(const std::string& competicion) {
        estadisticasPorCompeticion[competicion].partidosJugados += 1;
    }
    void aumentarGolesEnContra(const std::string& competicion, int cantidad) {
        estadisticasPorCompeticion[competicion].golesEnContra += cantidad;
    }

    Estadisticas getEstadisticas(const std::string& competicion) const {
        auto it = estadisticasPorCompeticion.find(competicion);
        if (it != estadisticasPorCompeticion.end()) {
            return it->second;
        } else {
            throw std::runtime_error("No hay estadísticas para la competición especificada.");
        }
    }
    const std::unordered_map<std::string, Estadisticas>& getEstadisticasPorCompeticion() const {
        return estadisticasPorCompeticion;
    }
    // Métodos para cálculos adicionales
    float obtenerPromedioGolesAFavor() const {
        return golesAFavorTotales / partidosJugados;
    };
    float obtenerPromedioGolesEnContra() const {
        return golesEnContraTotales / partidosJugados;
    };
};

#endif
