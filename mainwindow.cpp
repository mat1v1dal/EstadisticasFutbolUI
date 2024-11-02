#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "./servicios/ServicioPartido.h"
#include <vector>
#include "./dbcontroll/DataControl.h"
#include <QString>
#include <chrono>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qRegisterMetaType<Partido>("Partido");
    obtenerRutaArchivo();
    obtenerPartidos(servicioPartido, rutaArchivo);
    cargarPartidosEnListWidget();

    connect(ui->listWidget, &QListWidget::itemSelectionChanged, this, &MainWindow::onItemSelectionChanged);
    connect(ui->editButton, &QPushButton::clicked, this, &MainWindow::on_editarButton_clicked);
    connect(ui->top5Button, &QPushButton::clicked, this, [this]() {
        mostrarTop5Partidos(servicioPartido);
    });
    connect(ui->reiniciarWidget, &QPushButton::clicked, this, [this]() {
        resetWidget();
    });
    connect(ui->golesTotalesButton, &QPushButton::clicked, this, [this]() {
        mostrarGolesTotalesPorEquipo(servicioPartido);
    });
    connect(ui->promedioGolesButton, &QPushButton::clicked, this, [this]() {
        mostrarPromedioGolesPorEquipo(servicioPartido);
    });
    connect(ui->victoriasDerrotasButton, &QPushButton::clicked, this, [this]() {
        mostrarVictoriasYDerrotasPorCompeticion(servicioPartido);
    });
    connect(ui->fechaGolesButton, &QPushButton::clicked, this, [this]() {
        mostrarFechaConMasYMenosGoles(servicioPartido);
    });
    connect(ui->equipoMasGolesButton, &QPushButton::clicked, this, [this]() {
        equipoConMasGolesEnTodasLasCompeticiones(servicioPartido);
    });
    connect(ui->equipoMenosGolesButton, &QPushButton::clicked, this, [this]() {
        equipoConMenosGolesEnTodasLasCompeticiones(servicioPartido);
    });
    connect(ui->compararDosEquiposButton, &QPushButton::clicked, this, [this]() {
        compararRendimientoEquipos(servicioPartido);
    });
    ui->editButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);

}

void MainWindow::actualizarVectorDePartidos(const std::string& rutaArchivo){
    DataControl dataControl;
    partidos = dataControl.obtenerPartidos(rutaArchivo);
}

void MainWindow::obtenerRutaArchivo(){
    QString currentPath = QDir::currentPath();
    QString directory = QFileDialog::getOpenFileName(
        nullptr,
        "Seleccionar Archivo de Datos",
        currentPath,  // Usar el directorio actual como la ruta por defecto
        "Archivos CSV (*.csv);;Todos los archivos (*)"
        );

    rutaArchivo = directory.toStdString();

    if (directory.isEmpty()) {
        QMessageBox::warning(nullptr, "Sin selección", "No se seleccionó ninguna carpeta. La aplicación se cerrará.");
    }
}

void MainWindow::obtenerPartidos(ServicioPartidoTree &servicio, const std::string& rutaArchivo) {
    DataControl dataControl;

    auto start = std::chrono::high_resolution_clock::now();
    partidos = dataControl.obtenerPartidos(rutaArchivo);

    for(auto& partido : partidos) {
        servicio.registrarEquipo(partido.getLiga(), partido.getEquipoLocalObj());
        servicio.registrarEquipo(partido.getLiga(), partido.getEquipoVisitanteObj());
        servicio.actualizarEstadisticasPorCompeticion(partido);
        servicio.registrarPartidoEnHash(partido);
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
        item->setData(Qt::UserRole, QVariant::fromValue(partido));
        ui->listWidget->addItem(item);
    }
}

void MainWindow::limpiarListWidget(){
    ui->listWidget->clear();
}
void MainWindow::resetWidget(){
    limpiarListWidget();
    cargarPartidosEnListWidget();
}
void MainWindow::onItemSelectionChanged() {
    QListWidgetItem* currentItem = ui->listWidget->currentItem();
    ui->editButton->setEnabled(currentItem != nullptr);
    ui->deleteButton->setEnabled(currentItem != nullptr);
}

void MainWindow::on_editarButton_clicked() {
    QListWidgetItem* currentItem = ui->listWidget->currentItem();
    if (!currentItem) return;
    Partido partidoSeleccionado = currentItem->data(Qt::UserRole).value<Partido>();
    QString mensaje = QString("Detalles del partido seleccionado:\n"
                              "Jornada: %1\n"
                              "Liga: %2\n"
                              "Equipo Local: %3\n"
                              "Equipo Visitante: %4\n"
                              "Goles Local: %5\n"
                              "Goles Visitante: %6\n"
                              "Fecha: %7")
                          .arg(QString::fromStdString(partidoSeleccionado.getJornada()))
                          .arg(QString::fromStdString(partidoSeleccionado.getLiga()))
                          .arg(QString::fromStdString(partidoSeleccionado.getEquipoLocal()))
                          .arg(QString::fromStdString(partidoSeleccionado.getEquipoVisitante()))
                          .arg(partidoSeleccionado.getGolesLocal())
                          .arg(partidoSeleccionado.getGolesVisitante())
                          .arg(QString::fromStdString(partidoSeleccionado.getFecha().toString()));  // Suponiendo que Fecha tiene un método toString()

    // Mostrar el MessageBox con la información del partido
    QMessageBox::information(this, "Partido Seleccionado", mensaje);

    Partido partidoOriginal(partidoSeleccionado.getEquipoLocalObj(),
                            partidoSeleccionado.getEquipoVisitanteObj(),
                            partidoSeleccionado.getGolesLocal(),
                            partidoSeleccionado.getGolesVisitante(),
                            partidoSeleccionado.getLiga(),
                            partidoSeleccionado.getFecha());
    partidoOriginal.setJornada(partidoSeleccionado.getJornada());
    QString jornada = QInputDialog::getText(this, "Editar Partido", "Nueva jornada:", QLineEdit::Normal, QString::fromStdString(partidoSeleccionado.getJornada()));
    if (jornada.isEmpty()) {
        QMessageBox::warning(this, "Entrada Vacía", "El campo del equipo local no puede estar vacío.");
        return;
    }

    // Solicitar nuevo equipo local
    QString equipoLocal = QInputDialog::getText(this, "Editar Partido", "Nuevo Equipo Local:", QLineEdit::Normal, QString::fromStdString(partidoSeleccionado.getEquipoLocal()));
    if (equipoLocal.isEmpty()) {
        QMessageBox::warning(this, "Entrada Vacía", "El campo del equipo local no puede estar vacío.");
        return;
    }

    // Solicitar nuevo equipo visitante
    QString equipoVisitante = QInputDialog::getText(this, "Editar Partido", "Nuevo Equipo Visitante:", QLineEdit::Normal, QString::fromStdString(partidoSeleccionado.getEquipoVisitante()));
    if (equipoVisitante.isEmpty()) {
        QMessageBox::warning(this, "Entrada Vacía", "El campo del equipo visitante no puede estar vacío.");
        return;
    }

    // Solicitar nuevos goles del equipo local
    bool ok;
    int nuevosGolesLocal = QInputDialog::getInt(this, "Editar Partido", "Nuevos Goles Local:", partidoSeleccionado.getGolesLocal(), 0, 100, 1, &ok);
    if (!ok) return;  // El usuario canceló la entrada

    // Solicitar nuevos goles del equipo visitante
    int nuevosGolesVisitante = QInputDialog::getInt(this, "Editar Partido", "Nuevos Goles Visitante:", partidoSeleccionado.getGolesVisitante(), 0, 100, 1, &ok);
    if (!ok) return;  // El usuario canceló la entrada

    // Solicitar nueva fecha
    QString fechaStr = QInputDialog::getText(this, "Editar Partido", "Nueva Fecha (DD/MM/AAAA):", QLineEdit::Normal, QString::fromStdString(partidoSeleccionado.getFecha().toString()));
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
                           nuevosGolesLocal, nuevosGolesVisitante, partidoSeleccionado.getLiga(), nuevaFecha);
    partidoEditado.setJornada(jornada.toStdString());

    servicioPartido.actualizarPartido(partidoOriginal.getLiga(), partidoEditado, partidoOriginal);
    // Llamar a la función para editar el partido en la base de datos
    DataControl dataControl;
    dataControl.editarPartido(rutaArchivo, partidoOriginal, partidoEditado);

    partidos.clear();
    actualizarVectorDePartidos(rutaArchivo);
    limpiarListWidget();
    cargarPartidosEnListWidget();
    QMessageBox::information(this, "Aviso", QString("Partido editado correctamente"));
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

            item->setData(Qt::UserRole, QVariant::fromValue(partido));
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

void MainWindow::mostrarVictoriasYDerrotasPorCompeticion(ServicioPartidoTree& servicio) {
    // Solicitar el nombre del equipo al usuario
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

                resultados += QString("Competición: %1\nEquipo: %2\nVictorias: %3\nDerrotas: %4\n\n")
                                  .arg(QString::fromStdString(competicion))
                                  .arg(QString::fromStdString(equipo.getNombre()))
                                  .arg(stats.victorias)
                                  .arg(stats.derrotas);
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        if (!equipoEncontrado) {
            QMessageBox::information(this, "Equipo no encontrado", "El equipo no fue encontrado en ninguna competición.");
        } else {
            // Mostrar los resultados en un cuadro de mensaje
            QMessageBox::information(this, "Victorias y Derrotas por Competición", resultados);
        }
    } catch (...) {
        QMessageBox::warning(this, "Error", "No se pudo recuperar el equipo.");
    }



    // Mostrar el tiempo de ejecución
    QMessageBox::information(this, "Tiempo de ejecución", QString("Tiempo transcurrido: %1 segundos").arg(duration.count()));
}


void MainWindow::mostrarFechaConMasYMenosGoles(ServicioPartidoTree& servicio) {
    // Solicitar la competición
    bool ok;
    QString competicion = QInputDialog::getText(this, "Ingrese la Competición", "Competición:", QLineEdit::Normal, "", &ok);
    if (!ok || competicion.isEmpty()) {
        QMessageBox::warning(this, "Error", "No se ingresó ninguna competición.");
        return;
    }

    // Solicitar el nombre del equipo
    QString equipoNombre = QInputDialog::getText(this, "Ingrese Nombre del Equipo", "Equipo:", QLineEdit::Normal, "", &ok);
    if (!ok || equipoNombre.isEmpty()) {
        QMessageBox::warning(this, "Error", "No se ingresó ningún equipo.");
        return;
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration;
    try {
        // Obtener el partido con la fecha con más goles
        Partido partidoMasGoles = servicio.getFechaConMasGoles(equipoNombre.toStdString(), competicion.toStdString());
        QString resultadoMasGoles = QString("Fecha con más goles:\nEquipo Local: %1\nEquipo Visitante: %2\nGoles: %3 - %4\nFecha: %5\n\n")
                                        .arg(QString::fromStdString(partidoMasGoles.getEquipoLocal()))
                                        .arg(QString::fromStdString(partidoMasGoles.getEquipoVisitante()))
                                        .arg(partidoMasGoles.getGolesLocal())
                                        .arg(partidoMasGoles.getGolesVisitante())
                                        .arg(QString::fromStdString(partidoMasGoles.getFecha().toString()));

        // Obtener el partido con la fecha con menos goles
        Partido partidoMenosGoles = servicio.getFechaConMenosGoles(equipoNombre.toStdString(), competicion.toStdString());
        QString resultadoMenosGoles = QString("Fecha con menos goles:\nEquipo Local: %1\nEquipo Visitante: %2\nGoles: %3 - %4\nFecha: %5\n\n")
                                          .arg(QString::fromStdString(partidoMenosGoles.getEquipoLocal()))
                                          .arg(QString::fromStdString(partidoMenosGoles.getEquipoVisitante()))
                                          .arg(partidoMenosGoles.getGolesLocal())
                                          .arg(partidoMenosGoles.getGolesVisitante())
                                          .arg(QString::fromStdString(partidoMenosGoles.getFecha().toString()));
        auto end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        // Mostrar los resultados en un QMessageBox
        QMessageBox::information(this, "Resultados", resultadoMasGoles + "\n" + resultadoMenosGoles);
    } catch (...) {
        QMessageBox::warning(this, "Error", "No se pudo recuperar la competición o el equipo.");
    }



    // Mostrar el tiempo de ejecución
    QMessageBox::information(this, "Tiempo de ejecución", QString("Tiempo transcurrido: %1 segundos").arg(duration.count()));
}

void MainWindow::equipoConMasGolesEnTodasLasCompeticiones(ServicioPartidoTree& servicio) {
    auto start = std::chrono::high_resolution_clock::now();  // Inicio del cronómetro

    // Llamamos al servicio para obtener el equipo y la cantidad de goles
    auto [equipoMasGoles, totalGoles] = servicio.obtenerEquipoConMasGoles();

    auto end = std::chrono::high_resolution_clock::now();  // Fin del cronómetro
    std::chrono::duration<double> duration = end - start;

    // Mostrar el resultado incluyendo los goles
    QString resultado = QString("El equipo con más goles en todas las competiciones es: %1 con %2 goles.")
                            .arg(QString::fromStdString(equipoMasGoles.getNombre()))
                            .arg(totalGoles);

    resultado += QString("\n\nTiempo transcurrido: %1 segundos").arg(duration.count());
    QMessageBox::information(this, "Resultados", resultado);
}
void MainWindow::equipoConMenosGolesEnTodasLasCompeticiones(ServicioPartidoTree& servicio) {
    auto start = std::chrono::high_resolution_clock::now();  // Inicio del cronómetro

    // Llamamos al servicio para obtener el equipo y la cantidad de goles
    auto [equipoMasGoles, totalGoles] = servicio.obtenerEquipoConMasGoles();

    auto end = std::chrono::high_resolution_clock::now();  // Fin del cronómetro
    std::chrono::duration<double> duration = end - start;

    // Mostrar el resultado incluyendo los goles
    QString resultado = QString("El equipo con más goles en todas las competiciones es: %1 con %2 goles.")
                            .arg(QString::fromStdString(equipoMasGoles.getNombre()))
                            .arg(totalGoles);

    resultado += QString("\n\nTiempo transcurrido: %1 segundos").arg(duration.count());
    QMessageBox::information(this, "Resultados", resultado);
}

void MainWindow::compararRendimientoEquipos(ServicioPartidoTree& servicio) {
    QString equipo1 = QInputDialog::getText(this, "Equipo 1", "Ingrese el nombre del primer equipo:");
    QString equipo2 = QInputDialog::getText(this, "Equipo 2", "Ingrese el nombre del segundo equipo:");

    if (equipo1.isEmpty() || equipo2.isEmpty()) {
        QMessageBox::warning(this, "Error", "Debe ingresar nombres válidos para ambos equipos.");
        return;
    }
    auto start = std::chrono::high_resolution_clock::now();  // Inicio del cronómetro
    std::chrono::duration<double> duration;
    // Obtener partidos entre los dos equipos
    std::vector<Partido> partidos = servicio.obtenerPartidosEntreEquipos(equipo1.toStdString(), equipo2.toStdString());
    int victoriasEquipo1 = 0, victoriasEquipo2 = 0, empates = 0;

    limpiarListWidget();
    int startIdx = std::max(0, static_cast<int>(partidos.size()) - 5);
    for (int i = partidos.size() - 1; i >= startIdx; --i) {
        const auto& partido = partidos[i];

        QString itemText = QString("Jornada: %1 | Liga: %2 | %3 vs %4 | %5 - %6 | Fecha: %7")
                               .arg(QString::fromStdString(partido.getJornada()))
                               .arg(QString::fromStdString(partido.getLiga()))
                               .arg(QString::fromStdString(partido.getEquipoLocal()))
                               .arg(QString::fromStdString(partido.getEquipoVisitante()))
                               .arg(partido.getGolesLocal())
                               .arg(partido.getGolesVisitante())
                               .arg(QString::fromStdString(partido.getFecha().toString()));  // Suponiendo que Fecha tiene un método toString()

        QListWidgetItem* item = new QListWidgetItem(itemText);

        item->setData(Qt::UserRole, QVariant::fromValue(partido));
        ui->listWidget->addItem(item);
    }

    for (const auto& partido : partidos) {
        if (partido.getGolesLocal() == partido.getGolesVisitante()) {
            empates++;
        } else if ((partido.getEquipoLocal() == equipo1.toStdString() && partido.getGolesLocal() > partido.getGolesVisitante()) ||
                   (partido.getEquipoVisitante() == equipo1.toStdString() && partido.getGolesVisitante() > partido.getGolesLocal())) {
            victoriasEquipo1++;
        } else {
            victoriasEquipo2++;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();  // Fin del cronómetro
    duration = end - start;
    // Mostrar resultados
    QString resultado = QString("Partidos jugados: %1\nEmpates: %2\n")
                            .arg(partidos.size())
                            .arg(empates);
    resultado += QString("%1 ganó %2 partidos.\n").arg(equipo1).arg(victoriasEquipo1);
    resultado += QString("%1 ganó %2 partidos.\n").arg(equipo2).arg(victoriasEquipo2);

    QMessageBox::information(this, "Resultados de la Comparación", resultado);
    QMessageBox::information(this, "Tiempo de ejecución", QString("Tiempo transcurrido: %1 segundos").arg(duration.count()));

}

MainWindow::~MainWindow()
{
    delete ui;
}
