#ifndef SERVICIOPARTIDOTREE_H
#define SERVICIOPARTIDOTREE_H

#include "../entidades/partido.h"
#include "../entidades/equipo.h"
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include "../lib/ArbolBinarioAVL.h"
#include <iostream>
#include <vector>

class ServicioPartidoTree {
private:
    std::unordered_map<std::string, ArbolBinarioAVL<Partido>> competiciones;
    std::unordered_map<std::string, std::unordered_map<std::string, Equipo>> equipos;
    std::map<std::string, int> golesPorEquipo;
public:
    ServicioPartidoTree() {
    }

    void actualizarPartido(const std::string &liga, Partido& partido, Partido& partidoOriginal){
        auto it = competiciones.find(liga);
        if (it == competiciones.end()) throw std::runtime_error("No se encontro el partido");
        it->second.search(partidoOriginal).setAll(partido.getEquipoLocalObj(), partido.getEquipoVisitanteObj(), partido.getGolesLocal(), partido.getGolesVisitante(), partido.getLiga(), partido.getFecha(), partido.getJornada());
    }

    Partido getFechaConMenosGoles(const std::string& equipoNombre, const std::string& competicion) {
        try {
            ArbolBinarioAVL<Partido>& arbolPartidos = competiciones.at(competicion);
            std::vector<Partido> partidos = arbolPartidos.inorder();
            std::cout<<partidos.size()<<std::endl;
            for(const Partido& partido : partidos) {
                if(partido.getEquipoLocal() == equipoNombre || partido.getEquipoVisitante() == equipoNombre) {
                    return partido;
                }
            }
        } catch (const std::out_of_range&) {
            throw std::runtime_error("No se encontraron partidos para la competición.");
        }
    }
    Partido getFechaConMasGoles(const std::string& equipoNombre, const std::string& competicion) {
        try {
            ArbolBinarioAVL<Partido>& arbolPartidos = competiciones.at(competicion);
            std::vector<Partido> partidos = arbolPartidos.inorder();

            for(int i = partidos.size() - 1; i >= 0; --i) {
                if(partidos[i].getEquipoLocal() == equipoNombre || partidos[i].getEquipoVisitante() == equipoNombre) {
                    return partidos[i];
                }
            }
        } catch (const std::out_of_range&) {
            throw std::runtime_error("No se encontraron partidos para la competición.");
        }
    }
    void registrarEquipo(std::string liga, Equipo equipo) {
        try {
            equipos[liga].at(equipo.getNombre());
            equipos[liga].at(equipo.getNombre());
        } catch (const std::out_of_range&) {
            equipos[liga].insert({equipo.getNombre(), equipo});
        }
    }
    void actualizarEstadisticasPorCompeticion(Partido& partido) {
        std::string liga = partido.getLiga();

        // Acceder directamente al equipo local
        Equipo& equipoLocal = equipos[liga][partido.getEquipoLocal()];

        equipoLocal.aumentarGolesAFavor(liga, partido.getGolesLocal());
        equipoLocal.aumentarGolesEnContra(liga, partido.getGolesVisitante());
        equipoLocal.aumentarPartidosJugados(liga);
        if(partido.getGolesLocal() > partido.getGolesVisitante()){
            equipoLocal.registrarVictoria(liga);
        } else if(partido.getGolesLocal() < partido.getGolesVisitante()){
            equipoLocal.registrarDerrota(liga);
        }
        // Acceder directamente al equipo visitante
        Equipo& equipoVisitante = equipos[liga][partido.getEquipoVisitante()];

        equipoVisitante.aumentarGolesAFavor(liga, partido.getGolesVisitante());
        equipoVisitante.aumentarGolesEnContra(liga, partido.getGolesLocal());
        equipoVisitante.aumentarPartidosJugados(liga);
    }


    void registrarPartidoEnHash(const Partido& partido) {
        std::string key = partido.getLiga();

        // Acceder directamente al árbol AVL de la competición
        auto& arbolAVL = competiciones[key];
        // Si no existe la clave, se crea un nuevo ArbolBinarioAVL por defecto
        arbolAVL.put(partido);  // Insertar el partido en el árbol AVL
    }

    void cargarResultados(Partido& partido) {

        int golesLocal = partido.getGolesLocal();
        int golesVisitante = partido.getGolesVisitante();

        partido.getEquipoLocalObj().aumentarGolesAFavor(golesLocal);
        partido.getEquipoLocalObj().aumentarGolesEnContra(golesVisitante);

        partido.getEquipoVisitanteObj().aumentarGolesAFavor(golesVisitante);
        partido.getEquipoVisitanteObj().aumentarGolesEnContra(golesLocal);
    }
    const std::unordered_map<std::string, std::unordered_map<std::string, Equipo>>& getEquipos() const {
        return equipos;
    }
    Equipo& getEquipo(const std::string& competicion, const std::string& equipoNombre) {
        try {
            return equipos.at(competicion).at(equipoNombre);
        } catch (const std::out_of_range&) {
            throw std::runtime_error("Equipo no encontrado en la competición.");
        }
    }

    ArbolBinarioAVL<Partido> getPartidos(std::string competicion) {
        try {
            return competiciones.at(competicion);
        } catch (const std::out_of_range&) {
            throw std::runtime_error("No se encontraron partidos para la competición.");
        }
    }

    void registrarGolesPorEquipo(){
        for(const auto& [competicion, equiposEnCompeticion] : equipos){
            for(const auto& [nombre, equipo] : equiposEnCompeticion){
                golesPorEquipo[equipo.getNombre()] = equipo.getGolesAFavor();
            }
        }
    }

    std::map<std::string, int> getGolesPorEquipo(){
        return golesPorEquipo;
    }
};

#endif

