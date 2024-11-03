#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "servicios/ServicioPartido.h"
#include <QMainWindow>
#include <string>
#include <vector>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    std::string rutaArchivo;
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void obtenerRutaArchivo();
    void obtenerPartidos(ServicioPartidoTree &servicio,const std::string &rutaArchivo);
    void actualizarVectorDePartidos(const std::string& rutaArchivo);
    ServicioPartidoTree servicioPartido;
    std::vector<Partido> partidos;
    void cargarPartidosEnListWidget();
    void onItemSelectionChanged();
    void on_editarButton_clicked();
    void limpiarListWidget();
    void competicionConMasGoles(ServicioPartidoTree& servicio);
    void filtrarPartidosPorUmbral(ServicioPartidoTree& servicio);
    void mostrarEquiposConMasYMenosGoles(ServicioPartidoTree& servicio);
    void mostrarEquiposConMasYMenosGolesEnCompeticion(ServicioPartidoTree& servicio);
    void on_agregarButton_clicked();
    void buscarPartidosEntreFechasButton(ServicioPartidoTree& servicio);
    void mostrarTop5Partidos(ServicioPartidoTree& servicio);
    void mostrarGolesTotalesPorEquipo(ServicioPartidoTree& servicio);
    void mostrarPromedioGolesPorEquipo(ServicioPartidoTree& servicio);
    void mostrarVictoriasYDerrotasPorCompeticion(ServicioPartidoTree& servicio);
    void mostrarFechaConMasYMenosGoles(ServicioPartidoTree& servicio);
    void equipoConMasGolesEnTodasLasCompeticiones(ServicioPartidoTree& servicio);
    void equipoConMenosGolesEnTodasLasCompeticiones(ServicioPartidoTree& servicio);
    void compararRendimientoEquipos(ServicioPartidoTree& servicio);
    void resetWidget();
};
#endif // MAINWINDOW_H
