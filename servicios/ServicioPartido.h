#ifndef SERVICIOPARTIDOTREE_H
#define SERVICIOPARTIDOTREE_H

#include "../entidades/partido.h"
#include "../entidades/equipo.h"
#include <string>
#include <unordered_map>
#include <map>
#include <utility>
#include <vector>
#include "../lib/ArbolBinarioAVL.h"
#include <iostream>
#include <vector>

class ServicioPartidoTree {
private:
    std::unordered_map<std::string, ArbolBinarioAVL<Partido>> competiciones;
    std::unordered_map<std::string, std::unordered_map<std::string, Equipo>> equipos;
    int ultimoContadorIfs;
public:
    ServicioPartidoTree() {
    }

    int getContadorIfs() {
        return ultimoContadorIfs;
    }
    void eliminarPartido(Partido& partido){
        try {
            ArbolBinarioAVL<Partido>& arbolPartidos = competiciones.at(partido.getLiga());
            arbolPartidos.remove(partido);
        } catch (const std::out_of_range&) {
            throw std::runtime_error("No se encontraron partidos para la competición.");
        }
    }
    void actualizarPartido(const std::string &liga, Partido& partido, Partido& partidoOriginal){
        try {
            ArbolBinarioAVL<Partido>& arbolPartidos = competiciones.at(liga);
            arbolPartidos.remove(partidoOriginal);
            arbolPartidos.put(partido);
        } catch (const std::out_of_range&) {
            throw std::runtime_error("No se encontraron partidos para la competición.");
        }
    }

    Partido getFechaConMenosGoles(const std::string& equipoNombre, const std::string& competicion) {
        int contadorIfs = 0;
        try {
            ArbolBinarioAVL<Partido>& arbolPartidos = competiciones.at(competicion);
            std::vector<Partido> partidos = arbolPartidos.inorder();
            std::cout<<partidos.size()<<std::endl;
            for(const Partido& partido : partidos) {
                if(partido.getEquipoLocal() == equipoNombre || partido.getEquipoVisitante() == equipoNombre) {
                    ultimoContadorIfs = contadorIfs;
                    return partido;
                } contadorIfs+=2;
            }
        } catch (const std::out_of_range&) {
            throw std::runtime_error("No se encontraron partidos para la competición.");
        }
    }
    Partido getFechaConMasGoles(const std::string& equipoNombre, const std::string& competicion) {
        int contadorIfs=0;
        try {
            ArbolBinarioAVL<Partido>& arbolPartidos = competiciones.at(competicion);
            std::vector<Partido> partidos = arbolPartidos.inorder();

            for(int i = partidos.size() - 1; i >= 0; --i) {
                if(partidos[i].getEquipoLocal() == equipoNombre || partidos[i].getEquipoVisitante() == equipoNombre) {
                    ultimoContadorIfs = contadorIfs;
                    return partidos[i];
                }contadorIfs+=2;
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
        int contadorIfs = 0;
        std::string liga = partido.getLiga();

        // Acceder directamente al equipo local
        Equipo& equipoLocal = equipos[liga][partido.getEquipoLocal()];

        equipoLocal.aumentarGolesAFavor(liga, partido.getGolesLocal());


        equipoLocal.aumentarGolesEnContra(liga, partido.getGolesVisitante());


        equipoLocal.aumentarPartidosJugados(liga);
        if(partido.getGolesLocal() > partido.getGolesVisitante()){
            contadorIfs++;
            equipoLocal.registrarVictoria(liga);
        } else if(partido.getGolesLocal() < partido.getGolesVisitante()){
            contadorIfs++;
            equipoLocal.registrarDerrota(liga);
        }
        ultimoContadorIfs = contadorIfs;
        // Acceder directamente al equipo visitante
        Equipo& equipoVisitante = equipos[liga][partido.getEquipoVisitante()];

        equipoVisitante.aumentarGolesAFavor(liga, partido.getGolesVisitante());

        equipoVisitante.aumentarGolesEnContra(liga, partido.getGolesLocal());

        equipoVisitante.aumentarPartidosJugados(liga);
    }


    void registrarPartidoEnHash(Partido& partido) {
        std::string key = partido.getLiga();

        // Acceder directamente al árbol AVL de la competición
        auto& arbolAVL = competiciones[key];
        // Si no existe la clave, se crea un nuevo ArbolBinarioAVL por defecto
        arbolAVL.put(partido);  // Insertar el partido en el árbol AVL
    }

    void cargarResultados(Partido& partido) {

        int golesLocal = partido.getGolesLocal();
        int golesVisitante = partido.getGolesVisitante();

        partido.aumentarGolesLocal(golesLocal);

        partido.aumentarGolesVisitante(golesVisitante);

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
        ultimoContadorIfs = 0;
        try {
            return competiciones.at(competicion);
        } catch (const std::out_of_range&) {
            throw std::runtime_error("No se encontraron partidos para la competición.");
        }
    }

    std::pair<Equipo, int> obtenerEquipoConMasGoles() {
        int contadorIfs = 0;
        std::unordered_map<std::string, int> golesPorEquipo;
        Equipo equipoConMasGoles;
        int maxGoles = 0;

        for (const auto& [competicion, equiposEnCompeticion] : equipos) {
            for (const auto& [nombreEquipo, equipo] : equiposEnCompeticion) {
                Estadisticas stats = equipo.getEstadisticas(competicion);
                golesPorEquipo[nombreEquipo] += stats.golesAFavor;

                if (golesPorEquipo[nombreEquipo] > maxGoles) {
                    maxGoles = golesPorEquipo[nombreEquipo];
                    equipoConMasGoles = equipo;
                }contadorIfs++;
            }
        }
        ultimoContadorIfs = contadorIfs;
        return {equipoConMasGoles, maxGoles}; // Retornamos el equipo y sus goles totales
    }
    std::pair<Equipo, int> obtenerEquipoConMenosGoles() {
        int contadorIfs = 0;
        std::unordered_map<std::string, int> golesPorEquipo;
        Equipo equipoConMenosGoles;
        int minGoles = 999999;  // Inicializamos con el valor máximo de int

        for (const auto& [competicion, equiposEnCompeticion] : equipos) {
            for (const auto& [nombreEquipo, equipo] : equiposEnCompeticion) {
                Estadisticas stats = equipo.getEstadisticas(competicion);
                golesPorEquipo[nombreEquipo] += stats.golesAFavor;

                // Actualizamos el equipo con menos goles si encontramos uno con menor cantidad de goles
                if (golesPorEquipo[nombreEquipo] < minGoles) {
                    contadorIfs++;
                    minGoles = golesPorEquipo[nombreEquipo];
                    equipoConMenosGoles = equipo;
                }
            }
        }
        ultimoContadorIfs = contadorIfs;
        return {equipoConMenosGoles, minGoles}; // Retornamos el equipo y sus goles totales
    }
    std::vector<Partido> obtenerPartidosEntreEquipos(const std::string& equipo1, const std::string& equipo2) {
        int contadorIfs = 0;
        std::vector<Partido> partidosEntreEquipos;
        for (auto& [competicion, arbolPartidos] : competiciones) {
            std::vector<Partido> partidos = arbolPartidos.inorder();
            for (const auto& partido : partidos) {
                if ((partido.getEquipoLocal() == equipo1 && partido.getEquipoVisitante() == equipo2) ||
                    (partido.getEquipoLocal() == equipo2 && partido.getEquipoVisitante() == equipo1)) {
                    contadorIfs++;
                    partidosEntreEquipos.push_back(partido);
                }
            }
        }
        ultimoContadorIfs = contadorIfs;
        return partidosEntreEquipos;
    }

    std::pair<std::string, int> obtenerCompeticionConMasGoles() {
        int contadorIfs = 0;
        std::string competicionConMasGoles;
        int maxGoles = -1;

        // Iteramos sobre cada competición y sumamos los goles de sus equipos
        for (const auto& [nombreCompeticion, equiposEnCompeticion] : equipos) {
            int totalGolesCompeticion = 0;

            // Sumar los goles de cada equipo en la competición actual
            for (const auto& [nombreEquipo, equipo] : equiposEnCompeticion) {
                Estadisticas stats = equipo.getEstadisticas(nombreCompeticion);
                totalGolesCompeticion += stats.golesAFavor;
            }

            // Actualizamos si esta competición tiene más goles que las anteriores
            if (totalGolesCompeticion > maxGoles) {
                contadorIfs++;
                maxGoles = totalGolesCompeticion;
                competicionConMasGoles = nombreCompeticion;
            }
        }
        ultimoContadorIfs = contadorIfs;
        return {competicionConMasGoles, maxGoles};
    }

    std::pair<std::string, int> obtenerCompeticionConMenosGoles() {
        int contadorIfs = 0;
        std::string competicionConMenosGoles;
        int minGoles = 999999;

        // Iteramos sobre cada competición y sumamos los goles de sus equipos
        for (const auto& [nombreCompeticion, equiposEnCompeticion] : equipos) {
            int totalGolesCompeticion = 0;

            // Sumar los goles de cada equipo en la competición actual
            for (const auto& [nombreEquipo, equipo] : equiposEnCompeticion) {
                Estadisticas stats = equipo.getEstadisticas(nombreCompeticion);
                totalGolesCompeticion += stats.golesAFavor;
            }

            // Actualizamos si esta competición tiene menos goles que las anteriores
            if (totalGolesCompeticion < minGoles) {
                contadorIfs++;
                minGoles = totalGolesCompeticion;
                competicionConMenosGoles = nombreCompeticion;
            }
        }
        ultimoContadorIfs = contadorIfs;
        return {competicionConMenosGoles, minGoles};
    }

    std::pair<Equipo, int> obtenerEquipoConMasGolesEnCompeticion(const std::string& nombreCompeticion) {
        int contadorIfs = 0;
        Equipo equipoConMasGoles;
        int maxGoles = 0;

        // Verificamos si la competición ingresada existe en el mapa
        auto it = equipos.find(nombreCompeticion);
        if (it != equipos.end()) {
            for (const auto& [nombreEquipo, equipo] : it->second) {
                Estadisticas stats = equipo.getEstadisticas(nombreCompeticion);
                int goles = stats.golesAFavor;

                if (goles > maxGoles) {
                    contadorIfs++;
                    maxGoles = goles;
                    equipoConMasGoles = equipo;
                }
            }
        }
        ultimoContadorIfs = contadorIfs;
        return {equipoConMasGoles, maxGoles}; // Retornamos el equipo y sus goles totales
    }

    std::pair<Equipo, int> obtenerEquipoConMenosGolesEnCompeticion(const std::string& nombreCompeticion) {
        int contadorIfs = 0;
        Equipo equipoConMenosGoles;
        int minGoles = std::numeric_limits<int>::max(); // Inicializamos con el valor máximo de int

        // Verificamos si la competición ingresada existe en el mapa
        auto it = equipos.find(nombreCompeticion);
        if (it != equipos.end()) {
            for (const auto& [nombreEquipo, equipo] : it->second) {
                Estadisticas stats = equipo.getEstadisticas(nombreCompeticion);
                int goles = stats.golesAFavor;

                if (goles < minGoles) {
                    contadorIfs++;
                    minGoles = goles;
                    equipoConMenosGoles = equipo;
                }
            }
        }
        ultimoContadorIfs = contadorIfs;
        return {equipoConMenosGoles, minGoles}; // Retornamos el equipo y sus goles totales
    }
    std::vector<Partido> getPartidosPorUmbral(int umbral, bool side) {
        int contadorIfs = 0;
        std::vector<Partido> partidosFiltrados;

        // Recorremos el hash de competiciones
        for (const auto& [competicion, arbolPartidos] : competiciones) {
            // Obtener los partidos en orden
            std::vector<Partido> partidosLista =  competiciones[competicion].inorder();
            // Filtramos según el umbral
            if(side){
                for (int i = partidosLista.size() - 1; i >= 0; --i) {
                    // Supongamos que Partido tiene un método que devuelve los goles totales
                    int golesTotales = partidosLista[i].getGolesLocal() + partidosLista[i].getGolesVisitante();
                    if (golesTotales >= umbral) {
                        contadorIfs++;
                        partidosFiltrados.push_back(partidosLista[i]);
                    }
                    else{
                        break;
                    }
                }
            } else {
                for (int i = 0; i < partidosLista.size(); ++i) {
                    // Supongamos que Partido tiene un método que devuelve los goles totales
                    int golesTotales = partidosLista[i].getGolesLocal() + partidosLista[i].getGolesVisitante();
                    if (golesTotales <= umbral) {
                        contadorIfs++;
                        partidosFiltrados.push_back(partidosLista[i]);
                    }
                    else{
                        break;
                    }
                }
            }

        }
        ultimoContadorIfs = contadorIfs;
        return partidosFiltrados;
    }

    std::vector<Partido> getPartidosEntreFechas(std::string& equipo, Fecha& fechaInicio, Fecha& fechaFin){
        int contadorIfs = 0;
        std::vector<Partido> partidosEntreFechas;
        for(auto& [competicion, arbolPartidos] : competiciones) {
            std::vector<Partido> partidos = arbolPartidos.inorder();
            for(const Partido& partido : partidos) {
                if((partido.getEquipoLocal() == equipo || partido.getEquipoVisitante() == equipo) &&
                    ((partido.getFecha() > fechaInicio && partido.getFecha() < fechaFin)
                     || partido.getFecha() == fechaInicio || partido.getFecha() == fechaFin)) {
                    contadorIfs += 6;
                    partidosEntreFechas.push_back(partido);
                }
            }
        }
        ultimoContadorIfs = contadorIfs;
        return partidosEntreFechas;
    }
};

#endif

