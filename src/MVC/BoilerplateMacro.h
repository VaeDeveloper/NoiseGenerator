#pragma once


#define IMPL_TYPED_MODEL(Type, Field) \
    std::shared_ptr<Type> GetModel() override { return Field; } \
    std::shared_ptr<const Type> GetModel() const override { return Field; }

#define IMPL_MODEL_PTR_ACCESS(Type, Field) \
    Type* GetModel() { return Field.get(); } \
    const Type* GetModel() const { return Field.get(); }

#define IMPL_MODEL_REF_ACCESS(Type, Field) \
    Type& GetModel() { return *Field; } \
    const Type& GetModel() const { return *Field; }