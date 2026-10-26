
CC = gcc
CFLAGS =

# Check required parameters
ifeq ($(MODEL),)
$(error MODEL is required. Choose: chain_layers, small_layers, or large_layers)
endif

ifeq ($(PROPERTY),)
$(error PROPERTY is required. Choose: true or false)
endif

# Validate the model and select model-specific source files
ifeq ($(MODEL),chain_layers)
MODEL_SOURCES = model_chain_layers.c structure_variable_model_chain_layers.c
else ifeq ($(MODEL),small_layers)
MODEL_SOURCES = model_small_layers.c structure_variable_model_small_layers.c
else ifeq ($(MODEL),large_layers)
MODEL_SOURCES = model_large_layers.c structure_variable_model_large_layers.c
else
$(error Invalid MODEL '$(MODEL)'. Choose:  MODEL=chain_layers,  MODEL=small_layers, or  MODEL=large_layers)
endif

# Select the main source file according to the property
ifeq ($(PROPERTY),true)
MAIN_SOURCE = main_rep_true.c
else ifeq ($(PROPERTY),false)
MAIN_SOURCE = main_rep_false.c
else
$(error Invalid PROPERTY '$(PROPERTY)'. Choose: PROPERTY=true or PROPERTY=false )
endif

# Common source files
COMMON_SOURCES = ta_extended_builder.c \
                 DBM.c \
                 data_structures.c \
                 heuristiques_et_check.c

SOURCES = $(MAIN_SOURCE) $(COMMON_SOURCES) $(MODEL_SOURCES)
TARGET = executable_name

.PHONY: all clean

all:
	$(CC) $(CFLAGS) -DMODEL=$(MODEL) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET) $(TARGET).exe
