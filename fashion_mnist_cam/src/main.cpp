#include <Arduino.h>

void init_ml_module() {
 
  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  // NOLINTNEXTLINE(runtime-global-variables)
   static tflite::MicroErrorReporter micro_error_reporter;
   error_reporter = &micro_error_reporter;
  
  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  
  model = tflite::GetModel(_mnist_model);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report(
        "Model provided is schema version %d not equal "
        "to supported version %d.",
        model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // Here we define the model 
  static tflite::AllOpsResolver micro_mutable_op_resolver;
  // ----------------------
  
  interpreter =  new tflite::MicroInterpreter(
      model, micro_mutable_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
   
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    error_reporter -> Report("--- Error allocating tensor arena ----");
  }
 model_input = interpreter->input(0);
 model_input_buffer = model_input->data.f;

 // ESP_LOGI(TAG, "Model size %d", model_input->dims->size);
}

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}