#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

const char* estudiantes = "./instances/LSE91.stu";
const char* examenes = "./instances/LSE91.exm";

typedef struct {
        char id[20];
        int mis_examenes[60];
        int len_mis_examenes;
} STUDENT;

int main(int argc, char ** argv){

    FILE *exm, *stu;
    exm = fopen(examenes, "r");
    stu = fopen(estudiantes, "r");

    if (!exm || !stu) {
        printf("No se han podido abrir los archivos");
        return 1;
    }
    struct stat sb;
    if (stat(examenes, &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }
    char *file_contents = malloc(sb.st_size), *aux = malloc(sb.st_size);

    while (fscanf(exm, "%[^\n ] ", file_contents) != EOF && fscanf(exm, "%[^\n ] ", aux) != EOF) {
        //printf("> %s\n", file_contents);
    }
    int cant_examenes;
    cant_examenes = strtol(file_contents, NULL, 10); //este rescata el ultimo id de examen, que representa la cantidad total de examenes
    //-----------------se declara e inicializa la matriz de conflictos--------------------
    int matriz_conflictos[cant_examenes][cant_examenes];
    for(int i=0; i<cant_examenes; i++){
        for(int j=0; j<cant_examenes; j++){
            matriz_conflictos[i][j]=0;
        }
    }
    
    char *id_estudiante = malloc(sb.st_size), *id_examen = malloc(sb.st_size);
    STUDENT s={0};
    STUDENT* students; 
    students = (STUDENT*)malloc(sizeof(STUDENT));
    int len_students=0, len_examenes=0;
    
    //-------------Primera iteracion para construir el arreglo de structs students--------------------
    
    fscanf(stu, "%[^\n ] ", id_estudiante);
    fscanf(stu, "%[^\n ] ", id_examen);
    int examen_id;
    examen_id = strtol(id_examen, NULL, 10);
    strcpy(s.id, id_estudiante);
    s.mis_examenes[len_examenes]=examen_id;
    len_examenes++;
    s.len_mis_examenes = len_examenes;
    students[len_students]=s;
    //----------Construcción arreglo dinamico de struct student------------
    
    while (fscanf(stu, "%[^\n ] ", id_estudiante) != EOF && fscanf(stu, "%[^\n ] ", id_examen) != EOF) {
        examen_id = strtol(id_examen, NULL, 10);
        if (strcmp(id_estudiante, students[len_students].id)==0){
            students[len_students].mis_examenes[len_examenes]=examen_id;
            len_examenes++;
            students[len_students].len_mis_examenes = len_examenes;
        }
        else{
            STUDENT s={0};
            len_students++;
            len_examenes=0;
            students = (STUDENT*)realloc(students, (len_students+1)*sizeof(STUDENT));
            strcpy(s.id, id_estudiante);
            s.mis_examenes[len_examenes]=examen_id;
            len_examenes++;
            s.len_mis_examenes = len_examenes;
            students[len_students]=s;
        }
        
    }
    
    /*printf("len students: %d\n", len_students);

    for(int i=0; i<= len_students; i++){
        printf("id estudiante: %s\n", students[i].id);
        printf("len mis examenes: %d\n", students[i].len_mis_examenes);
        for(int j=0; j<students[i].len_mis_examenes; j++){
            printf("examen a rendir: %d\n", students[i].mis_examenes[j]);
        }
        printf("\n");
    }*/

    //----------------------Armado de matriz de conflictos-----------------------------
    for(int i=0; i<= len_students; i++){
        for(int j=0; j<students[i].len_mis_examenes; j++){
            //printf("examen a rendir: %d\n", students[i].mis_examenes[j]);
            if(students[i].len_mis_examenes>1){
                for(int k=j+1; k<students[i].len_mis_examenes; k++){
                    matriz_conflictos[students[i].mis_examenes[j]-1][students[i].mis_examenes[k]-1] = 1;
                    matriz_conflictos[students[i].mis_examenes[k]-1][students[i].mis_examenes[j]-1] = 1;
                }
            }
            
        }
        //printf("\n");
    }

    /*for(int a=0; a<cant_examenes; a++){
        for(int b=0; b<cant_examenes; b++){
            printf("%d ", matriz_conflictos[a][b]);
        }
        printf("\n");
    }*/

    int solucion[cant_examenes];
    int cant_timeslots_actual = 1;

    //------------------solucion inicial greedy -------------------------
    for(int columna=0; columna<cant_examenes; columna++){
        if(matriz_conflictos[0][columna]==0){
            solucion[columna]=1;
        }
        else{
            solucion[columna]=2;
            cant_timeslots_actual = 2;
        }       
    }

    //----------------Algoritmo greedy para encontrar una solucion inicial-----------------------------
    int flag = 0; //flag que indica si se debe aumentar la cantidad de timeslots actual
    for(int sol=0; sol<cant_examenes; sol++){
        for(int solu=sol+1; solu<cant_examenes; solu++){
            if(solucion[sol]==solucion[solu]){
                if(matriz_conflictos[sol][solu]==1){
                    flag=1;
                    solucion[solu]=cant_timeslots_actual+1;
                }
            }
        }
        if(flag == 1){
            cant_timeslots_actual++;
        }
        flag = 0;
    }

    for(int j=1; j<=cant_timeslots_actual; j++){
        printf("timeslot %d :\n", j);
        for(int i=0; i<cant_examenes; i++){
            if(solucion[i]==j){
                printf("prueba %d\n", i+1);
            }
            
        }
        printf("\n");
    }
    
    //printf("\n");
    //printf("cantidad examenes: %d\n", cant_examenes);
    printf("cantidad de slots necesarios: %d\n", cant_timeslots_actual);
    //printf("len students: %d\n", len_students);
    //printf("len examenes estudiante %s : %d\n", students[175].id, students[175].len_mis_examenes);
    //printf("examen %d\n", students[175].mis_examenes[2]);



    

    

    fclose(exm);
    fclose(stu);
    return 0;
}