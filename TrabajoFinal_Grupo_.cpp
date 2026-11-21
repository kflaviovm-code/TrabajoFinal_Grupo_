// TrabajoFinal_Grupo_.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//
// ==========================================================
//   SISTEMA DE GESTION DE DELIVERY INTELIGENTE
//   Solo usa: funciones, condicionales y bucles (for, while, do while)
// ==========================================================
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <iomanip>
using namespace std;

const int MAX_PEDIDOS = 200;
const int NUM_REP = 4;

// ---------- DATOS DE PEDIDOS (arreglos paralelos) ----------
string codigo[MAX_PEDIDOS];
string cliente[MAX_PEDIDOS];
string distrito[MAX_PEDIDOS];
double monto[MAX_PEDIDOS];
string tipo[MAX_PEDIDOS];
string estado[MAX_PEDIDOS];
string repAsignado[MAX_PEDIDOS];   // codigo del repartidor ("---" si no tiene)
int totalPedidos = 0;

// ---------- DATOS DE REPARTIDORES ----------
string codRep[NUM_REP] = { "R001", "R002", "R003", "R004" };
string nomRep[NUM_REP] = { "Carlos", "Ana", "Luis", "Rosa" };
int entregasRep[NUM_REP] = { 0, 0, 0, 0 };

// ==========================================================
//   FUNCIONES AUXILIARES
// ==========================================================
string aMayusculas(string t) {
    for (int i = 0; i < t.length(); i++) {
        t[i] = toupper(t[i]);
    }
    return t;
}

string leerTexto(string mensaje) {
    string t;
    cout << mensaje;
    getline(cin, t);
    return t;
}

int leerEntero(string mensaje) {
    return atoi(leerTexto(mensaje).c_str());
}

double leerReal(string mensaje) {
    return atof(leerTexto(mensaje).c_str());
}

// Devuelve la posicion del pedido o -1 si no existe
int buscarPedido(string cod) {
    for (int i = 0; i < totalPedidos; i++) {
        if (codigo[i] == cod) {
            return i;
        }
    }
    return -1;
}

// Devuelve la posicion del repartidor o -1 si no existe
int buscarRepartidor(string cod) {
    for (int i = 0; i < NUM_REP; i++) {
        if (codRep[i] == cod) {
            return i;
        }
    }
    return -1;
}

// Un repartidor esta ocupado si tiene algun pedido "En Ruta"
bool repartidorOcupado(string cod) {
    for (int i = 0; i < totalPedidos; i++) {
        if (repAsignado[i] == cod && estado[i] == "En Ruta") {
            return true;
        }
    }
    return false;
}

// MODULO 3: costo del delivery segun el tipo
double costoDelivery(string t) {
    if (t == "Normal") {
        return 5;
    }
    else if (t == "Express") {
        return 10;
    }
    else if (t == "Premium") {
        return 15;
    }
    return 0;
}

// Separa una linea "A|B|C" en partes. Devuelve cuantas partes encontro
int dividirLinea(string linea, string partes[], int maxPartes) {
    int n = 0;
    string actual = "";
    for (int i = 0; i < linea.length(); i++) {
        if (linea[i] == '|') {
            if (n < maxPartes) {
                partes[n] = actual;
            }
            n++;
            actual = "";
        }
        else {
            actual += linea[i];
        }
    }
    if (n < maxPartes) {
        partes[n] = actual;
    }
    n++;
    return n;
}

// ==========================================================
//   MODULO 7: ARCHIVOS DE TEXTO
// ==========================================================
void guardarPedidos() {
    ofstream archivo("pedidos.txt");
    if (!archivo) {
        cout << "Error: no se pudo abrir pedidos.txt\n";
        return;
    }
    for (int i = 0; i < totalPedidos; i++) {
        archivo << codigo[i] << "|" << cliente[i] << "|" << distrito[i] << "|"
            << monto[i] << "|" << tipo[i] << "|" << estado[i] << "|"
            << repAsignado[i] << "\n";
    }
    archivo.close();
}

void guardarRepartidores() {
    ofstream archivo("repartidores.txt");
    if (!archivo) {
        cout << "Error: no se pudo abrir repartidores.txt\n";
        return;
    }
    for (int i = 0; i < NUM_REP; i++) {
        archivo << codRep[i] << "|" << nomRep[i] << "|" << entregasRep[i] << "\n";
    }
    archivo.close();
}

void cargarPedidos() {
    ifstream archivo("pedidos.txt");
    if (!archivo) {
        cout << "(No existe pedidos.txt todavia, se creara al registrar un pedido)\n";
        return;
    }
    totalPedidos = 0;
    string linea;
    while (getline(archivo, linea)) {
        if (linea != "" && totalPedidos < MAX_PEDIDOS) {
            string p[7];
            int n = dividirLinea(linea, p, 7);
            if (n >= 6) {
                codigo[totalPedidos] = p[0];
                cliente[totalPedidos] = p[1];
                distrito[totalPedidos] = p[2];
                monto[totalPedidos] = atof(p[3].c_str());
                tipo[totalPedidos] = p[4];
                estado[totalPedidos] = p[5];
                if (n >= 7 && p[6] != "") {
                    repAsignado[totalPedidos] = p[6];
                }
                else {
                    repAsignado[totalPedidos] = "---";
                }
                totalPedidos++;
            }
        }
    }
    archivo.close();
    cout << "Pedidos cargados: " << totalPedidos << "\n";
}

void cargarRepartidores() {
    ifstream archivo("repartidores.txt");
    if (!archivo) {
        return;   // se usan los valores por defecto
    }
    string linea;
    while (getline(archivo, linea)) {
        if (linea != "") {
            string p[3];
            int n = dividirLinea(linea, p, 3);
            if (n >= 3) {
                int pos = buscarRepartidor(p[0]);
                if (pos != -1) {
                    entregasRep[pos] = atoi(p[2].c_str());
                }
            }
        }
    }
    archivo.close();
}

// Historial: se agrega una linea al finalizar una entrega
void generarHistorial(int pos) {
    ofstream historial("historial.txt", ios::app);
    if (!historial) {
        cout << "Error: no se pudo abrir historial.txt\n";
        return;
    }
    double total = monto[pos] + costoDelivery(tipo[pos]);
    historial << codigo[pos] << "|" << cliente[pos] << "|" << distrito[pos] << "|"
        << total << "|" << repAsignado[pos] << "|Entregado\n";
    historial.close();
}

// ==========================================================
//   MODULO 1: REGISTRO DE PEDIDOS
// ==========================================================
void registrarPedido() {
    cout << "\n--- REGISTRAR PEDIDO ---\n";
    if (totalPedidos >= MAX_PEDIDOS) {
        cout << "No hay espacio para mas pedidos.\n";
        return;
    }

    // Codigo (no vacio y no repetido)
    string cod;
    bool valido = false;
    do {
        cod = aMayusculas(leerTexto("Codigo de pedido (ej. P001): "));
        if (cod == "") {
            cout << "El codigo no puede estar vacio.\n";
        }
        else if (buscarPedido(cod) != -1) {
            cout << "Ese codigo ya existe. Intente con otro.\n";
        }
        else {
            valido = true;
        }
    } while (!valido);

    string nombre = leerTexto("Nombre del cliente: ");
    string dist = leerTexto("Distrito: ");

    double m;
    do {
        m = leerReal("Monto de compra (S/.): ");
        if (m <= 0) {
            cout << "El monto debe ser mayor a 0.\n";
        }
    } while (m <= 0);

    int op;
    do {
        cout << "Tipo de entrega:\n";
        cout << "  1. Normal  (S/.5)\n";
        cout << "  2. Express (S/.10)\n";
        cout << "  3. Premium (S/.15)\n";
        op = leerEntero("Seleccione: ");
        if (op < 1 || op > 3) {
            cout << "Opcion invalida.\n";
        }
    } while (op < 1 || op > 3);

    string t;
    if (op == 1) {
        t = "Normal";
    }
    else if (op == 2) {
        t = "Express";
    }
    else {
        t = "Premium";
    }

    codigo[totalPedidos] = cod;
    cliente[totalPedidos] = nombre;
    distrito[totalPedidos] = dist;
    monto[totalPedidos] = m;
    tipo[totalPedidos] = t;
    estado[totalPedidos] = "Pendiente";
    repAsignado[totalPedidos] = "---";
    totalPedidos++;

    guardarPedidos();   // se guarda al registrar
    cout << "Pedido " << cod << " registrado y guardado correctamente.\n";
}

// ==========================================================
//   MOSTRAR PEDIDOS
// ==========================================================
void mostrarPedidos() {
    cout << "\n--- LISTA DE PEDIDOS ---\n";
    if (totalPedidos == 0) {
        cout << "No hay pedidos registrados.\n";
        return;
    }
    cout << left << setw(8) << "Codigo" << setw(18) << "Cliente"
        << setw(16) << "Distrito" << setw(9) << "Monto"
        << setw(10) << "Tipo" << setw(12) << "Estado" << "Repartidor\n";
    cout << "---------------------------------------------------------------------------\n";
    for (int i = 0; i < totalPedidos; i++) {
        cout << left << setw(8) << codigo[i] << setw(18) << cliente[i]
            << setw(16) << distrito[i] << setw(9) << monto[i]
            << setw(10) << tipo[i] << setw(12) << estado[i]
            << repAsignado[i] << "\n";
    }
}

// ==========================================================
//   MODULO 5: BUSQUEDA DE PEDIDOS
// ==========================================================
void buscarPedidoMenu() {
    cout << "\n--- BUSCAR PEDIDO ---\n";
    string cod = aMayusculas(leerTexto("Ingrese codigo: "));
    int pos = buscarPedido(cod);
    if (pos == -1) {
        cout << "No se encontro el pedido " << cod << ".\n";
    }
    else {
        cout << "Cliente: " << cliente[pos] << "\n";
        cout << "Distrito: " << distrito[pos] << "\n";
        cout << "Estado: " << estado[pos] << "\n";
    }
}

// ==========================================================
//   MODULO 2: ASIGNACION DE REPARTIDORES
// ==========================================================
void mostrarRepartidores() {
    cout << "\n--- REPARTIDORES ---\n";
    for (int i = 0; i < NUM_REP; i++) {
        cout << codRep[i] << " - " << left << setw(8) << nomRep[i]
            << " Disponible = ";
        if (repartidorOcupado(codRep[i])) {
            cout << "NO\n";
        }
        else {
            cout << "SI\n";
        }
    }
}

// Busca el primer repartidor libre y se lo asigna al pedido "pos"
bool asignarAutomatico(int pos) {
    bool asignado = false;
    int i = 0;
    while (i < NUM_REP && !asignado) {
        if (!repartidorOcupado(codRep[i])) {      // Disponible = SI
            repAsignado[pos] = codRep[i];
            estado[pos] = "En Ruta";
            asignado = true;
            cout << "Repartidor asignado: " << codRep[i] << " - " << nomRep[i] << "\n";
        }
        else {                                  // Disponible = NO
            cout << nomRep[i] << " esta ocupado, buscando otro...\n";
        }
        i++;
    }
    if (!asignado) {
        cout << "No hay repartidores disponibles por ahora.\n";
    }
    return asignado;
}

void asignarRepartidorMenu() {
    cout << "\n--- ASIGNAR REPARTIDOR ---\n";
    mostrarRepartidores();
    string cod = aMayusculas(leerTexto("\nCodigo del pedido: "));
    int pos = buscarPedido(cod);
    if (pos == -1) {
        cout << "No se encontro el pedido.\n";
    }
    else if (estado[pos] != "Pendiente") {
        cout << "Solo se puede asignar a pedidos Pendientes (estado actual: "
            << estado[pos] << ").\n";
    }
    else {
        if (asignarAutomatico(pos)) {
            guardarPedidos();
            cout << "El pedido paso a estado En Ruta.\n";
        }
    }
}

// ==========================================================
//   MODULO 4: CONTROL DE ENTREGAS
// ==========================================================
void actualizarEstado() {
    cout << "\n--- ACTUALIZAR ESTADO ---\n";
    string cod = aMayusculas(leerTexto("Codigo del pedido: "));
    int pos = buscarPedido(cod);

    if (pos == -1) {
        cout << "No se encontro el pedido.\n";
        return;
    }
    cout << "Estado actual: " << estado[pos] << "\n";

    if (estado[pos] == "Entregado" || estado[pos] == "Cancelado") {
        cout << "Este pedido ya esta finalizado, no se puede modificar.\n";
        return;
    }

    int op;
    do {
        cout << "Nuevo estado:\n";
        cout << "  1. En Ruta\n";
        cout << "  2. Entregado\n";
        cout << "  3. Cancelado\n";
        op = leerEntero("Seleccione: ");
        if (op < 1 || op > 3) {
            cout << "Opcion invalida.\n";
        }
    } while (op < 1 || op > 3);

    if (op == 1) {
        if (estado[pos] == "En Ruta") {
            cout << "El pedido ya esta En Ruta.\n";
        }
        else if (asignarAutomatico(pos)) {
            cout << "Estado actualizado a En Ruta.\n";
        }
    }
    else if (op == 2) {
        if (estado[pos] != "En Ruta") {
            cout << "El pedido debe estar En Ruta antes de marcarse como Entregado.\n";
        }
        else {
            estado[pos] = "Entregado";
            int r = buscarRepartidor(repAsignado[pos]);
            if (r != -1) {
                entregasRep[r]++;
            }
            generarHistorial(pos);      // historial al finalizar la entrega
            guardarRepartidores();
            cout << "Pedido entregado. Se agrego al historial.txt\n";
        }
    }
    else {
        estado[pos] = "Cancelado";
        cout << "Pedido cancelado.\n";
    }
    guardarPedidos();
}

// ==========================================================
//   MODULO 3: CALCULAR DELIVERY
// ==========================================================
void calcularDelivery() {
    cout << "\n--- CALCULAR DELIVERY ---\n";
    string cod = aMayusculas(leerTexto("Codigo del pedido: "));
    int pos = buscarPedido(cod);
    if (pos == -1) {
        cout << "No se encontro el pedido.\n";
    }
    else {
        double delivery = costoDelivery(tipo[pos]);
        double total = monto[pos] + delivery;
        cout << "Tipo de entrega: " << tipo[pos] << "\n";
        cout << "Monto de compra: S/. " << monto[pos] << "\n";
        cout << "Costo delivery : S/. " << delivery << "\n";
        cout << "TOTAL A PAGAR  : S/. " << total << "\n";
    }
}

// ==========================================================
//   MODULO 6: REPORTES
// ==========================================================
void reportes() {
    int entregados = 0;
    int pendientes = 0;
    double ventas = 0;

    for (int i = 0; i < totalPedidos; i++) {
        if (estado[i] == "Entregado") {
            entregados++;
            ventas += monto[i];      // venta = pedidos entregados
        }
        else if (estado[i] == "Pendiente") {
            pendientes++;
        }
    }

    int mejor = 0;
    for (int i = 1; i < NUM_REP; i++) {
        if (entregasRep[i] > entregasRep[mejor]) {
            mejor = i;
        }
    }

    cout << "\n=========== REPORTES ===========\n";
    cout << "Total pedidos: " << totalPedidos << "\n";
    cout << "Entregados: " << entregados << "\n";
    cout << "Pendientes: " << pendientes << "\n";
    cout << "Ventas acumuladas: S/. " << ventas << "\n";
    if (entregasRep[mejor] > 0) {
        cout << nomRep[mejor] << ": " << entregasRep[mejor] << " entregas\n";
    }
    else {
        cout << "Repartidor con mas entregas: aun no hay entregas.\n";
    }
    cout << "================================\n";
}

// ==========================================================
//   MENU PRINCIPAL
// ==========================================================
void mostrarMenu() {
    cout << "\n=============================\n";
    cout << "  SISTEMA DELIVERY INTELIGENTE\n";
    cout << "=============================\n";
    cout << "1. Registrar Pedido\n";
    cout << "2. Mostrar Pedidos\n";
    cout << "3. Buscar Pedido\n";
    cout << "4. Asignar Repartidor\n";
    cout << "5. Actualizar Estado\n";
    cout << "6. Calcular Delivery\n";
    cout << "7. Reportes\n";
    cout << "8. Guardar Datos\n";
    cout << "9. Cargar Datos\n";
    cout << "0. Salir\n";
}

int main() {
    // Leer pedidos al iniciar el sistema
    cargarPedidos();
    cargarRepartidores();

    int opcion;
    do {
        mostrarMenu();
        opcion = leerEntero("Seleccione: ");

        if (opcion == 1) {
            registrarPedido();
        }
        else if (opcion == 2) {
            mostrarPedidos();
        }
        else if (opcion == 3) {
            buscarPedidoMenu();
        }
        else if (opcion == 4) {
            asignarRepartidorMenu();
        }
        else if (opcion == 5) {
            actualizarEstado();
        }
        else if (opcion == 6) {
            calcularDelivery();
        }
        else if (opcion == 7) {
            reportes();
        }
        else if (opcion == 8) {
            guardarPedidos();
            guardarRepartidores();
            cout << "Datos guardados correctamente.\n";
        }
        else if (opcion == 9) {
            cargarPedidos();
            cargarRepartidores();
        }
        else if (opcion == 0) {
            guardarPedidos();
            guardarRepartidores();
            cout << "Datos guardados. Hasta pronto!\n";
        }
        else {
            cout << "Opcion invalida, intente de nuevo.\n";
        }
    } while (opcion != 0);

    return 0;
}
