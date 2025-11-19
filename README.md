# MAI_OOP
LabN VariantN.
# Build
Launch container:
```bash
docker compose up
```
Log into container with bash shell:
```bash
docker exec -it <container_id or label> bash
```
Build with cmake:
```bash
mkdir build
cd build
cmake ..
cmake --build .
```
Launch tests with `./tests`. 

