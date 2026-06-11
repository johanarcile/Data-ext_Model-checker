# Modifier les variables du TA

Ce document recense toutes les modifications à faire dans `structure_state_space_ta.fbs`, `save.c` et `visualize_state_space.py` selon les changements apportés à la structure `Variable` côté C.

---

## Code actuel

Une seule variable int `v` :

```fbs
table Variable {
    v:int;
}
```

```c
// save.c
State_space_ta_Variable_create(&B, s->var.v)
```

```python
# visualize_state_space.py
var_v = var.V() if var is not None else None
label = f"... v={var_v}"
```

---

## Renommer la variable

`.fbs` :

```fbs
table Variable {
    nom:int;
}
```

`save.c` :

```c
State_space_ta_Variable_create(&B, s->var.nom)
```

`visualize_state_space.py` :

```python
# FlatBuffers génère les méthodes Python en PascalCase
# ex : nom → Nom(), var_x → VarX()
var_v = var.Nom() if var is not None else None
label = f"... nom={var_v}"
```

---

## Changer le type : `int` → `float`

`.fbs` :

```fbs
table Variable {
    v:float;
}
```

`save.c` :

```c
State_space_ta_Variable_create(&B, (float)s->var.v)
```

`visualize_state_space.py` : aucun changement.

---

## Ajouter une variable

### Deux entiers

`.fbs` :

```fbs
table Variable {
    v:int;
    w:int;
}
```

`save.c` :

```c
State_space_ta_Variable_create(&B, s->var.v, s->var.w)
```

`visualize_state_space.py` :

```python
var_v = var.V() if var is not None else None
var_w = var.W() if var is not None else None
label = f"... v={var_v}\n w={var_w}"
```

---

### Un entier et un float

`.fbs` :

```fbs
table Variable {
    v:int;
    w:float;
}
```

`save.c` :

```c
State_space_ta_Variable_create(&B, s->var.v, (float)s->var.w)
```

`visualize_state_space.py` :

```python
var_v = var.V() if var is not None else None
var_w = var.W() if var is not None else None
label = f"... v={var_v}\n w={var_w}"
```

---

## Variable tableau

### Tableau d'entiers

`.fbs` :

```fbs
table Variable {
    v:[int];
}
```

`save.c` :

```c
// N = taille du tableau
flatbuffers_int32_vec_ref_t v_ref = flatbuffers_int32_vec_create(&B, s->var.v, N);
State_space_ta_Variable_create(&B, v_ref)
```

`visualize_state_space.py` :

```python
var_v = [var.V(i) for i in range(var.VLength())] if var is not None else []
v_str = "\n ".join(f"v[{i}]={x}" for i, x in enumerate(var_v))
label = f"ID: {state_id}, {loc_name}\n {constraints_str}\n {v_str}"
```

---

### Tableau de floats

`.fbs` :

```fbs
table Variable {
    v:[float];
}
```

`save.c` :

```c
flatbuffers_float_vec_ref_t v_ref = flatbuffers_float_vec_create(&B, s->var.v, N);
State_space_ta_Variable_create(&B, v_ref)
```

`visualize_state_space.py` : identique au cas tableau d'entiers.
