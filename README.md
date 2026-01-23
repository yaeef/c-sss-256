# SSS-512  
## Esquema de Secreto Compartido de Shamir (512 bits)

Este proyecto implementa el **Esquema de Secreto Compartido de Shamir** (*Shamir's Secret Sharing*) utilizando aritmética de precisión arbitraria (**BigNum**) sobre un **Campo Finito de Galois GF(P)** de **512 bits**.

El sistema permite dividir un secreto `S` en `n` fragmentos (*shares*), de tal forma que se requieren **al menos `k` fragmentos** para reconstruir el secreto original.
Cualquier intento con `k − 1` fragmentos **no revela información matemática** sobre el secreto, garantizando **Seguridad Teórica de la Información**.

---
