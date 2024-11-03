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
    // Unordered map para almacenar competiciones con sus respectivos árboles AVL de partidos.
    // La clave es el nombre de la competición y el valor es un árbol AVL de partidos de dicha competición.
    // Se utiliza un unordered_map para obtener en O(1) la lista de partidos de cada competición.
    std::unordered_map<std::string, ArbolBinarioAVL<Partido>> competiciones;
    // Unordered map anidado para almacenar equipos por competición.
    // La clave externa es el nombre de la competición y la clave interna es el nombre del equipo.
    // Cada equipo se almacena en un unordered_map para acceso rápido en O(1) por nombre.
    std::unordered_map<std::string, std::unordered_map<std::string, Equipo>> equipos;
    int ultimoContadorIfs;
public:
    // Constructor de la clase
    ServicioPartidoTree() {}

    // Método para obtener el último contador de condiciones 'if' que se ejecutaron.
    int getContadorIfs() {
        return ultimoContadorIfs;
    }

    // Método para eliminar un partido de una competición específica.
    // Accede al `unordered_map` `competiciones` y busca el árbol AVL de la competición.
    void eliminarPartido(Partido& partido) {
        try {
            // Acceso al árbol AVL en O(1)
            ArbolBinarioAVL<Partido>& arbolPartidos = competiciones.at(partido.getLiga());
            // Eliminación en el árbol AVL en O(log n)
            arbolPartidos.remove(partido);
        } catch (const std::out_of_range&) {
            throw std::runtime_error("No se encontraron partidos para la competición.");
        }
    }

    // Método para actualizar un partido en una competición.
    // Accede al árbol AVL de `competiciones` para eliminar y agregar el partido actualizado.
    void actualizarPartido(const std::string &liga, Partido& partido, Partido& partidoOriginal) {
        try {
            // Acceso al árbol AVL de la competición en O(1)
            ArbolBinarioAVL<Partido>& arbolPartidos = competiciones.at(liga);
            // Operaciones de eliminación e inserción en O(log n)
            arbolPartidos.remove(partidoOriginal);
            arbolPartidos.put(partido);
        } catch (const std::out_of_range&) {
            throw std::runtime_error("No se encontraron partidos para la competición.");
        }
    }

    // Método para obtener el partido con menos goles de un equipo en una competición.
    // Accede al árbol AVL y realiza un recorrido inorder (O(n)).
    Partido getFechaConMenosGoles(const std::string& equipoNombre, const std::string& competicion) {
        int contadorIfs = 0;
        try {
            ArbolBinarioAVL<Partido>& arbolPartidos = competiciones.at(competicion);
            // Recorrido inorder para obtener los partidos en orden (O(n))
            std::vector<Partido> partidos = arbolPartidos.inorder();
            for (const Partido& partido : partidos) {
                // Comparaciones para encontrar el partido con menos goles (O(n))
                if (partido.getEquipoLocal() == equipoNombre || partido.getEquipoVisitante() == equipoNombre) {
                    ultimoContadorIfs = contadorIfs;
                    return partido;
                }
                contadorIfs += 2; // Contador de condiciones evaluadas
            }
        } catch (const std::out_of_range&) {
            throw std::runtime_error("No se encontraron partidos para la competición.");
        }
    }

    // Similar al método anterior, pero encuentra el partido con más goles.
    Partido getFechaConMasGoles(const std::string& equipoNombre, const std::string& competicion) {
        int contadorIfs = 0;
        try {
            ArbolBinarioAVL<Partido>& arbolPartidos = competiciones.at(competicion);
            std::vector<Partido> partidos = arbolPartidos.inorder();
            for (int i = partidos.size() - 1; i >= 0; --i) {
                if (partidos[i].getEquipoLocal() == equipoNombre || partidos[i].getEquipoVisitante() == equipoNombre) {
                    ultimoContadorIfs = contadorIfs;
                    return partidos[i];
                }
                contadorIfs += 2;
            }
        } catch (const std::out_of_range&) {
            throw std::runtime_error("No se encontraron partidos para la competición.");
        }
    }

    // Método para registrar un equipo en una competición.
    // Inserta en el `unordered_map` anidado `equipos` en O(1).
    void registrarEquipo(std::string liga, Equipo equipo) {
        try {
            equipos[liga].at(equipo.getNombre()); // Verifica si ya existe el equipo.
        } catch (const std::out_of_range&) {
            // Inserción en el `unordered_map` en O(1)
            equipos[liga].insert({equipo.getNombre(), equipo});
        }
    }

    // Método para actualizar las estadísticas de un equipo en una competición.
    void actualizarEstadisticasPorCompeticion(Partido& partido) {
        int contadorIfs = 0;
        std::string liga = partido.getLiga();
        // Acceso al equipo local en O(1)
        Equipo& equipoLocal = equipos[liga][partido.getEquipoLocal()];
        // Actualización de estadísticas
        equipoLocal.aumentarGolesAFavor(liga, partido.getGolesLocal());
        equipoLocal.aumentarGolesEnContra(liga, partido.getGolesVisitante());
        equipoLocal.aumentarPartidosJugados(liga);

        // Evaluación de condiciones para registrar victoria o derrota.
        if (partido.getGolesLocal() > partido.getGolesVisitante()) {
            contadorIfs++;
            equipoLocal.registrarVictoria(liga);
        } else if (partido.getGolesLocal() < partido.getGolesVisitante()) {
            contadorIfs++;
            equipoLocal.registrarDerrota(liga);
        }
        ultimoContadorIfs = contadorIfs;

        // Acceso al equipo visitante y actualización similar al local.
        Equipo& equipoVisitante = equipos[liga][partido.getEquipoVisitante()];
        equipoVisitante.aumentarGolesAFavor(liga, partido.getGolesVisitante());
        equipoVisitante.aumentarGolesEnContra(liga, partido.getGolesLocal());
        equipoVisitante.aumentarPartidosJugados(liga);
    }

    // Método para registrar un partido en el hash de `competiciones`.
    void registrarPartidoEnHash(Partido& partido) {
        std::string key = partido.getLiga();
        // Acceso al árbol AVL y creación si no existe (O(1) para acceso, O(log n) para inserción)
        auto& arbolAVL = competiciones[key];
        arbolAVL.put(partido);
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
        // Itera sobre las competiciones y los equipos para calcular el total de goles.
        for (const auto& [competicion, equiposEnCompeticion] : equipos) {
            for (const auto& [nombreEquipo, equipo] : equiposEnCompeticion) {
                Estadisticas stats = equipo.getEstadisticas(competicion);
                golesPorEquipo[nombreEquipo] += stats.golesAFavor;
                // Actualiza el equipo con más goles si el total actual supera al máximo.
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
        // Itera sobre las competiciones y los equipos para calcular el total de goles.
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

    // Método para obtener el equipo con más goles en una competición específica.
    // Retorna un par que contiene el equipo con más goles y la cantidad de goles.
    std::pair<Equipo, int> obtenerEquipoConMasGolesEnCompeticion(const std::string& nombreCompeticion) {
        int contadorIfs = 0;  // Contador para seguimiento de comparaciones.
        Equipo equipoConMasGoles;  // Variable para almacenar el equipo con más goles.
        int maxGoles = 0;  // Inicialización del máximo de goles.

        // Verifica si la competición existe en el mapa de equipos.
        auto it = equipos.find(nombreCompeticion);
        if (it != equipos.end()) {  // Si la competición se encuentra...
            for (const auto& [nombreEquipo, equipo] : it->second) {  // Itera sobre los equipos de la competición.
                Estadisticas stats = equipo.getEstadisticas(nombreCompeticion);
                int goles = stats.golesAFavor;  // Obtiene los goles a favor del equipo.

                // Si los goles del equipo actual superan al máximo registrado, actualiza el máximo y el equipo.
                if (goles > maxGoles) {
                    contadorIfs++;
                    maxGoles = goles;
                    equipoConMasGoles = equipo;
                }
            }
        }
        ultimoContadorIfs = contadorIfs;  // Almacena el contador de comparaciones.
        return {equipoConMasGoles, maxGoles};  // Retorna el equipo con más goles y la cantidad de goles.
    }

    // Método para obtener el equipo con menos goles en una competición específica.
    // Retorna un par que contiene el equipo con menos goles y la cantidad de goles.
    std::pair<Equipo, int> obtenerEquipoConMenosGolesEnCompeticion(const std::string& nombreCompeticion) {
        int contadorIfs = 0;  // Contador para seguimiento de comparaciones.
        Equipo equipoConMenosGoles;  // Variable para almacenar el equipo con menos goles.
        int minGoles = std::numeric_limits<int>::max();  // Inicialización con un valor alto.

        // Verifica si la competición existe en el mapa de equipos.
        auto it = equipos.find(nombreCompeticion);
        if (it != equipos.end()) {  // Si la competición se encuentra...
            for (const auto& [nombreEquipo, equipo] : it->second) {  // Itera sobre los equipos de la competición.
                Estadisticas stats = equipo.getEstadisticas(nombreCompeticion);
                int goles = stats.golesAFavor;  // Obtiene los goles a favor del equipo.

                // Si los goles del equipo actual son menores que el mínimo registrado, actualiza el mínimo y el equipo.
                if (goles < minGoles) {
                    contadorIfs++;
                    minGoles = goles;
                    equipoConMenosGoles = equipo;
                }
            }
        }
        ultimoContadorIfs = contadorIfs;  // Almacena el contador de comparaciones.
        return {equipoConMenosGoles, minGoles};  // Retorna el equipo con menos goles y la cantidad de goles.
    }

    // Método para obtener partidos con un umbral específico de goles.
    // Retorna una lista de partidos que cumplen con el umbral.
    std::vector<Partido> getPartidosPorUmbral(int umbral, bool side) {
        int contadorIfs = 0;  // Contador para seguimiento de comparaciones.
        std::vector<Partido> partidosFiltrados;  // Vector para almacenar los partidos que cumplen con el criterio.

        // Itera sobre todas las competiciones y sus árboles de partidos.
        for (const auto& [competicion, arbolPartidos] : competiciones) {
            std::vector<Partido> partidosLista = competiciones[competicion].inorder();  // Obtiene los partidos en orden.

            // Filtra partidos dependiendo de la opción 'side' (true para >=, false para <=).
            if (side) {
                for (int i = partidosLista.size() - 1; i >= 0; --i) {
                    int golesTotales = partidosLista[i].getGolesLocal() + partidosLista[i].getGolesVisitante();
                    if (golesTotales >= umbral) {  // Si los goles totales son mayores o iguales al umbral...
                        contadorIfs++;
                        partidosFiltrados.push_back(partidosLista[i]);  // Añade el partido a la lista.
                    } else {
                        break;  // Detiene la búsqueda si ya no se cumplen los criterios.
                    }
                }
            } else {
                for (int i = 0; i < partidosLista.size(); ++i) {
                    int golesTotales = partidosLista[i].getGolesLocal() + partidosLista[i].getGolesVisitante();
                    if (golesTotales <= umbral) {  // Si los goles totales son menores o iguales al umbral...
                        contadorIfs++;
                        partidosFiltrados.push_back(partidosLista[i]);  // Añade el partido a la lista.
                    } else {
                        break;  // Detiene la búsqueda si ya no se cumplen los criterios.
                    }
                }
            }
        }
        ultimoContadorIfs = contadorIfs;  // Almacena el contador de comparaciones.
        return partidosFiltrados;  // Retorna la lista de partidos filtrados.
    }

    // Método para obtener partidos de un equipo entre dos fechas específicas.
    // Retorna una lista de partidos que se jugaron entre las fechas proporcionadas.
    std::vector<Partido> getPartidosEntreFechas(std::string& equipo, Fecha& fechaInicio, Fecha& fechaFin) {
        int contadorIfs = 0;  // Contador para seguimiento de comparaciones.
        std::vector<Partido> partidosEntreFechas;  // Vector para almacenar los partidos que cumplen con las fechas.

        // Itera sobre todas las competiciones y sus árboles de partidos.
        for (auto& [competicion, arbolPartidos] : competiciones) {
            std::vector<Partido> partidos = arbolPartidos.inorder();  // Obtiene los partidos en orden.

            // Filtra los partidos que involucran al equipo y están entre las fechas dadas.
            for (const Partido& partido : partidos) {
                if ((partido.getEquipoLocal() == equipo || partido.getEquipoVisitante() == equipo) &&
                    ((partido.getFecha() > fechaInicio && partido.getFecha() < fechaFin) ||
                     partido.getFecha() == fechaInicio || partido.getFecha() == fechaFin)) {
                    contadorIfs += 6;
                    partidosEntreFechas.push_back(partido);  // Añade el partido a la lista.
                }
            }
        }
        ultimoContadorIfs = contadorIfs;  // Almacena el contador de comparaciones.
        return partidosEntreFechas;  // Retorna la lista de partidos.
    }
};

#endif

