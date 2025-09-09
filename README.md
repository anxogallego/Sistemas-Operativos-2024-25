# Sistemas Operativos 2024-25

Práctica de la asignatura de **Sistemas Operativos** de la Universidad de A Coruña (UDC) en el grado de Ingeniería Informática, curso 2024/2025.  
Código de la asignatura: **GEI-SO 614G01016**.

---

## Calificaciones de Prácticas
- **P1:** 4.1/10  
  *(Razón: Violación de segmento en la función `historic N` (función de Labassignment0), por lo cual aspirábamos a un máximo de 5. Nota equiparable sin esa funcion: 8.2)*
- **P2:** 9.4  
- **P3:** 8.2  

---

## Compilación

Actualmente, el archivo `main` está definido en `p3.c`. Por lo tanto:
- En el `Makefile`, se indica `main` como `p3.c`.
- El ejecutable que se crea al hacer `make` en nuestro terminal en nuestra carpeta con el proyecto se llama `p3`.

### Cambiar el archivo principal
Si deseas usar otro archivo como `main`:
1. Cambia la referencia de `main` en el `Makefile` al archivo que prefieras.
2. Modifica el nombre del ejecutable según lo necesites.

---

## Implementación de las listas
Para nuestro proyecto empleamos la opción *Array of Pointers* para la mayoría de las listas, excepto aquellas que exigían una implementación específica por requerimientos del profesor.

---

## Autor

> **[fu:s1cknex0](https://www.dc.fi.udc.es/~so-grado/)**

