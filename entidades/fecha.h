#ifndef FECHA_H
#define FECHA_H
#include <string>
class Fecha {
private:
    int dia;
    int mes;
    int anio;

public:
    Fecha(int dia, int mes, int anio) : dia(dia), mes(mes), anio(anio) {}

    int getDia() const {
        return dia;
    }

    std::string toString(){
        return std::to_string(dia) + "/" + std::to_string(mes) + "/" + std::to_string(anio);
    }

    int getMes() const {
        return mes;
    }
    int getAnio() const {
        return anio;
    }

    void setDia(int dia) {
        this->dia = dia;
    }
    void setMes(int mes) {
        this->mes = mes;
    }
    void setAnio(int anio) {
        this->anio = anio;
    }

    // Sobrecarga de operador ==
    bool operator==(const Fecha& fecha) const {
        return dia == fecha.getDia() && mes == fecha.getMes() && anio == fecha.getAnio();
    }

    // Sobrecarga de operador <
    bool operator<(const Fecha& fecha) const {
        if (anio < fecha.getAnio()) {
            return true;
        } else if (anio == fecha.getAnio()) {
            if (mes < fecha.getMes()) {
                return true;
            } else if (mes == fecha.getMes()) {
                return dia < fecha.getDia();
            }
        }
        return false;
    }

    // Sobrecarga de operador >
    bool operator>(const Fecha& fecha) const {
        if (anio > fecha.getAnio()) {
            return true;
        } else if (anio == fecha.getAnio()) {
            if (mes > fecha.getMes()) {
                return true;
            } else if (mes == fecha.getMes()) {
                return dia > fecha.getDia();
            }
        }
        return false;
    }
};

#endif
