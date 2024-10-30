#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "servicios/ServicioPartido.h"
#include <vector>
#include "dbcontroll/DataControl.h"
#include <QString>
#include <chrono>
#include <QMessageBox>
#include <QInputDialog>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    obtenerPartidos(servicioPartido, rutaArchivo);
    cargarPartidosEnListWidget();

    connect(ui->listWidget, &QListWidget::itemSelectionChanged, this, &MainWindow::onItemSelectionChanged);
    connect(ui->editButton, &QPushButton::clicked, this, &MainWindow::on_editarButton_clicked);
    connect(ui->top5Button, &QPushButton::clicked, this, [this]() {
        mostrarTop5Partidos(servicioPartido);
    });
    connect(ui->golesTotalesButton, &QPushButton::clicked, this, [this]() {
        mostrarGolesTotalesPorEquipo(servicioPartido);
    });
    connect(ui->promedioGolesButton, &QPushButton::clicked, this, [this]() {
        mostrarPromedioGolesPorEquipo(servicioPartido);
    });
    ui->editButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);

}

void MainWindow::actualizarVectorDePartidos(const std::string& rutaArchivo){
    DataControl dataControl;
    partidos = dataControl.obtenerPartidos(rutaArchivo);
}


void MainWindow::obtenerPartidos(ServicioPartidoTree &servicio, const std::string& rutaArchivo) {
    DataControl dataControl;

    auto start = std::chrono::high_resolution_clock::now();
    partidos = dataControl.obtenerPartidos(rutaArchivo);

    for(auto& partido : partidos) {
        servicio.registrarEquipo(partido.getLiga(), partido.getEquipoLocalObj());
        servicio.registrarEquipo(partido.getLiga(), partido.getEquipoVisitanteObj());
        servicio.registrarPartidoEnHash(partido);
        servicio.actualizarEstadisticasPorCompeticion(partido);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    // Mostrar mensaje de éxito con el tiempo de carga
    QMessageBox::information(this, "Carga Completa",
                             QString("Partidos cargados correctamente.\nTiempo de carga: %1 segundos")
                                 .arg(duration.count()));
}

void MainWindow::cargarPartidosEnListWidget() {
    for (const auto& partido : partidos) {
        QString itemText = QString("Jornada: %1 | Liga: %2 | %3 vs %4 | %5 - %6 | Fecha: %7")
            .arg(QString::fromStdString(partido.getJornada()))
            .arg(QString::fromStdString(partido.getLiga()))
            .arg(QString::fromStdString(partido.getEquipoLocal()))
            .arg(QString::fromStdString(partido.getEquipoVisitante()))
            .arg(partido.getGolesLocal())
            .arg(partido.getGolesVisitante())
            .arg(QString::fromStdString(partido.getFecha().toString()));  // Suponiendo que Fecha tiene un método toString() que devuelve una fecha legible.

        QListWidgetItem* item = new QListWidgetItem(itemText);

        // Guardamos un puntero al objeto Partido asociado al item
        item->setData(Qt::UserRole, QVariant::fromValue(reinterpret_cast<quintptr>(&partido)));
        ui->listWidget->addItem(item);
    }
}

void MainWindow::limpiarListWidget(){
    ui->listWidget->clear();
}
void MainWindow::onItemSelectionChanged() {
    QListWidgetItem* currentItem = ui->listWidget->currentItem();
    ui->editButton->setEnabled(currentItem != nullptr);
    ui->deleteButton->setEnabled(currentItem != nullptr);
}

void MainWindow::on_editarButton_clicked() {
    QListWidgetItem* currentItem = ui->listWidget->currentItem();
    if (!currentItem) return;

    auto partidoSeleccionado = reinterpret_cast<Partido*>(currentItem->data(Qt::UserRole).value<quintptr>());
    Partido partidoOriginal(partidoSeleccionado->getEquipoLocalObj(),
                            partidoSeleccionado->getEquipoVisitanteObj(),
                            partidoSeleccionado->getGolesLocal(),
                            partidoSeleccionado->getGolesVisitante(),
                            partidoSeleccionado->getLiga(),
                            partidoSeleccionado->getFecha());
    if (!partidoSeleccionado) return;

    QString jornada = QInputDialog::getText(this, "Editar Partido", "Nueva jornada:", QLineEdit::Normal, QString::fromStdString(partidoSeleccionado->getJornada()));
    if (jornada.isEmpty()) {
        QMessageBox::warning(this, "Entrada Vacía", "El campo del equipo local no puede estar vacío.");
        return;
    }
    // Solicitar nuevo equipo local
    QString equipoLocal = QInputDialog::getText(this, "Editar Partido", "Nuevo Equipo Local:", QLineEdit::Normal, QString::fromStdString(partidoSeleccionado->getEquipoLocal()));
    if (equipoLocal.isEmpty()) {
        QMessageBox::warning(this, "Entrada Vacía", "El campo del equipo local no puede estar vacío.");
        return;
    }

    // Solicitar nuevo equipo visitante
    QString equipoVisitante = QInputDialog::getText(this, "Editar Partido", "Nuevo Equipo Visitante:", QLineEdit::Normal, QString::fromStdString(partidoSeleccionado->getEquipoVisitante()));
    if (equipoVisitante.isEmpty()) {
        QMessageBox::warning(this, "Entrada Vacía", "El campo del equipo visitante no puede estar vacío.");
        return;
    }

    // Solicitar nuevos goles del equipo local
    bool ok;
    int nuevosGolesLocal = QInputDialog::getInt(this, "Editar Partido", "Nuevos Goles Local:", partidoSeleccionado->getGolesLocal(), 0, 100, 1, &ok);
    if (!ok) return;  // El usuario canceló la entrada

    // Solicitar nuevos goles del equipo visitante
    int nuevosGolesVisitante = QInputDialog::getInt(this, "Editar Partido", "Nuevos Goles Visitante:", partidoSeleccionado->getGolesVisitante(), 0, 100, 1, &ok);
    if (!ok) return;  // El usuario canceló la entrada

    // Solicitar nueva fecha
    QString fechaStr = QInputDialog::getText(this, "Editar Partido", "Nueva Fecha (DD/MM/AAAA):", QLineEdit::Normal, QString::fromStdString(partidoSeleccionado->getFecha().toString()));
    if (fechaStr.isEmpty()) {
        QMessageBox::warning(this, "Entrada Vacía", "El campo de la fecha no puede estar vacío.");
        return;
    }

    // Validar y parsear la fecha ingresada
    QStringList fechaParts = fechaStr.split('/');
    if (fechaParts.size() != 3 || fechaParts[0].toInt(&ok) < 1 || fechaParts[0].toInt(&ok) > 31 ||
        fechaParts[1].toInt(&ok) < 1 || fechaParts[1].toInt(&ok) > 12 ||
        fechaParts[2].toInt(&ok) < 1900) {
        QMessageBox::warning(this, "Fecha Inválida", "Por favor, ingrese una fecha válida (DD/MM/AAAA).");
        return;
    }

    int dia = fechaParts[0].toInt();
    int mes = fechaParts[1].toInt();
    int anio = fechaParts[2].toInt();

    Fecha nuevaFecha(dia, mes, anio); // Asumiendo que tienes un constructor en Fecha que recibe dia, mes, año

    // Crear el objeto de partido editado
    Partido partidoEditado(Equipo(equipoLocal.toStdString()), Equipo(equipoVisitante.toStdString()),
                           nuevosGolesLocal, nuevosGolesVisitante, partidoSeleccionado->getLiga(), nuevaFecha);
    partidoEditado.setJornada(jornada.toStdString());
    // Llamar a la función para editar el partido en la base de datos
    DataControl dataControl;
    dataControl.editarPartido(rutaArchivo, partidoOriginal, partidoEditado);
    servicioPartido.actualizarPartido(partidoOriginal.getLiga(), partidoEditado, partidoOriginal);
    partidos.clear();
    actualizarVectorDePartidos(rutaArchivo);
    limpiarListWidget();
    cargarPartidosEnListWidget();
    QMessageBox::information(this, "Aviso",
                             QString("Partido editado correctamente"));

}


void MainWindow::mostrarTop5Partidos(ServicioPartidoTree& servicio){
    bool ok;
    QString competicion = QInputDialog::getText(this, "Ingrese Competición", "Competición:", QLineEdit::Normal, "", &ok);

    if (!ok || competicion.isEmpty()) {
        QMessageBox::warning(this, "Error", "No se ingresó ninguna competición.");
        return;
    }

    auto start = std::chrono::high_resolution_clock::now();

    try {
        ArbolBinarioAVL<Partido> partidos = servicio.getPartidos(competicion.toStdString());
        std::vector<Partido> partidosLista = partidos.inorder();  // Obtener partidos en orden ascendente

        if (partidosLista.empty()) {
            QMessageBox::information(this, "Sin resultados", "No se encontraron partidos en la competición ingresada.");
            return;
        }

        // Limpiar el `listWidget` antes de cargar los nuevos partidos
        limpiarListWidget();

        // Mostrar los 5 partidos con más goles al final de la lista ordenada
        int startIdx = std::max(0, static_cast<int>(partidosLista.size()) - 5);
        for (int i = partidosLista.size() - 1; i >= startIdx; --i) {
            const auto& partido = partidosLista[i];

            QString itemText = QString("Jornada: %1 | Liga: %2 | %3 vs %4 | %5 - %6 | Fecha: %7")
                                   .arg(QString::fromStdString(partido.getJornada()))
                                   .arg(QString::fromStdString(partido.getLiga()))
                                   .arg(QString::fromStdString(partido.getEquipoLocal()))
                                   .arg(QString::fromStdString(partido.getEquipoVisitante()))
                                   .arg(partido.getGolesLocal())
                                   .arg(partido.getGolesVisitante())
                                   .arg(QString::fromStdString(partido.getFecha().toString()));  // Suponiendo que Fecha tiene un método toString()

            QListWidgetItem* item = new QListWidgetItem(itemText);

            // Guardamos un puntero al objeto Partido asociado al item
            item->setData(Qt::UserRole, QVariant::fromValue(reinterpret_cast<quintptr>(&partido)));
            ui->listWidget->addItem(item);
        }
    } catch (...) {
        QMessageBox::warning(this, "Error", "No se pudo recuperar la competición.");
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    QMessageBox::information(this, "Tiempo de ejecución", QString("Tiempo transcurrido: %1 segundos").arg(duration.count()));
}

void MainWindow::mostrarGolesTotalesPorEquipo(ServicioPartidoTree& servicio) {
    // Pedir al usuario que ingrese el nombre del equipo
    bool ok;
    QString equipoNombre = QInputDialog::getText(this, "Ingrese Nombre del Equipo", "Equipo:", QLineEdit::Normal, "", &ok);

    if (!ok || equipoNombre.isEmpty()) {
        QMessageBox::warning(this, "Error", "No se ingresó ningún equipo.");
        return;
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration;
    try {
        bool equipoEncontrado = false;
        QString resultados;

        // Buscar el equipo en todas las competiciones
        for (const auto& [competicion, equiposEnCompeticion] : servicio.getEquipos()) {
            auto it = equiposEnCompeticion.find(equipoNombre.toStdString());
            if (it != equiposEnCompeticion.end()) {
                equipoEncontrado = true;
                const Equipo& equipo = it->second;
                Estadisticas stats = equipo.getEstadisticas(competicion);

                resultados += QString("Competición: %1\nEquipo: %2\nGoles a favor: %3\nGoles en contra: %4\nPartidos jugados: %5\n\n")
                                  .arg(QString::fromStdString(competicion))
                                  .arg(QString::fromStdString(equipo.getNombre()))
                                  .arg(stats.golesAFavor)
                                  .arg(stats.golesEnContra)
                                  .arg(stats.partidosJugados);
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        if (!equipoEncontrado) {
            QMessageBox::information(this, "Equipo no encontrado", "El equipo no fue encontrado en ninguna competición.");
        } else {
            // Mostrar los resultados en un cuadro de mensaje
            QMessageBox::information(this, "Estadísticas del Equipo", resultados);
        }
    } catch (...) {
        QMessageBox::warning(this, "Error", "No se pudo recuperar el equipo.");
    }



    // Mostrar el tiempo de ejecución
    QMessageBox::information(this, "Tiempo de ejecución", QString("Tiempo transcurrido: %1 segundos").arg(duration.count()));
}

void MainWindow::mostrarPromedioGolesPorEquipo(ServicioPartidoTree& servicio) {
    // Solicitar al usuario el nombre del equipo
    bool ok;
    QString equipoNombre = QInputDialog::getText(this, "Ingrese Nombre del Equipo", "Equipo:", QLineEdit::Normal, "", &ok);

    if (!ok || equipoNombre.isEmpty()) {
        QMessageBox::warning(this, "Error", "No se ingresó ningún equipo.");
        return;
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration;
    try {
        bool equipoEncontrado = false;
        QString resultados;

        // Buscar el equipo en todas las competiciones
        for (const auto& [competicion, equiposEnCompeticion] : servicio.getEquipos()) {
            auto it = equiposEnCompeticion.find(equipoNombre.toStdString());
            if (it != equiposEnCompeticion.end()) {
                equipoEncontrado = true;
                const Equipo& equipo = it->second;
                Estadisticas stats = equipo.getEstadisticas(competicion);

                float promedioFavor = stats.partidosJugados > 0 ? static_cast<float>(stats.golesAFavor) / stats.partidosJugados : 0;
                float promedioContra = stats.partidosJugados > 0 ? static_cast<float>(stats.golesEnContra) / stats.partidosJugados : 0;

                resultados += QString("Competición: %1\nEquipo: %2\nPromedio de goles a favor: %3\nPromedio de goles en contra: %4\n\n")
                                  .arg(QString::fromStdString(competicion))
                                  .arg(QString::fromStdString(equipo.getNombre()))
                                  .arg(promedioFavor, 0, 'f', 2)
                                  .arg(promedioContra, 0, 'f', 2);
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        if (!equipoEncontrado) {
            QMessageBox::information(this, "Equipo no encontrado", "El equipo no fue encontrado en ninguna competición.");
        } else {
            // Mostrar los resultados en un cuadro de mensaje
            QMessageBox::information(this, "Promedio de Goles por Equipo", resultados);
        }
    } catch (...) {
        QMessageBox::warning(this, "Error", "No se pudo recuperar el equipo.");
    }



    // Mostrar el tiempo de ejecución
    QMessageBox::information(this, "Tiempo de ejecución", QString("Tiempo transcurrido: %1 segundos").arg(duration.count()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
