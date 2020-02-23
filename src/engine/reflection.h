#pragma once


#include "engine/lumix.h"
#include "engine/metaprogramming.h"
#include "engine/resource.h"
#include "engine/stream.h"
#include "engine/string.h"
#include "engine/universe.h"


#define LUMIX_PROP(Scene, Property) &Scene::get##Property, &Scene::set##Property
#define LUMIX_ENUM_VALUE(value) EnumValue(#value, (int)value)
#define LUMIX_FUNC(Func)\
	&Func, #Func

namespace Lumix
{


template <typename T> struct Array;
struct IAllocator;
struct Path;
struct PropertyDescriptorBase;
struct IVec2;
struct IVec3;
struct Vec2;
struct Vec3;
struct Vec4;


namespace Reflection
{


struct IAttribute
{
	enum Type
	{
		MIN,
		CLAMP,
		RADIANS,
		COLOR,
		RESOURCE
	};

	virtual ~IAttribute() {}
	virtual int getType() const = 0;
};

struct ComponentBase;
struct IPropertyVisitor;
struct SceneBase;
struct EnumBase;

struct IAttributeVisitor
{
	virtual ~IAttributeVisitor() {}
	virtual void visit(const IAttribute& attr) = 0;
};

struct PropertyBase
{
	virtual ~PropertyBase() {}

	virtual void visit(IAttributeVisitor& visitor) const = 0;
	virtual void setValue(ComponentUID cmp, int index, InputMemoryStream& stream) const = 0;
	virtual void getValue(ComponentUID cmp, int index, OutputMemoryStream& stream) const = 0;

	const char* name;
};


LUMIX_ENGINE_API void init(IAllocator& allocator);
LUMIX_ENGINE_API void shutdown();

LUMIX_ENGINE_API int getEnumsCount();
LUMIX_ENGINE_API const EnumBase& getEnum(int index);

LUMIX_ENGINE_API const IAttribute* getAttribute(const PropertyBase& prop, IAttribute::Type type);
LUMIX_ENGINE_API void registerEnum(const EnumBase& e);
LUMIX_ENGINE_API void registerScene(const SceneBase& scene);
LUMIX_ENGINE_API const ComponentBase* getComponent(ComponentType cmp_type);
LUMIX_ENGINE_API const PropertyBase* getProperty(ComponentType cmp_type, const char* property);
LUMIX_ENGINE_API const PropertyBase* getProperty(ComponentType cmp_type, u32 property_name_hash);
LUMIX_ENGINE_API const PropertyBase* getProperty(ComponentType cmp_type, const char* property, const char* subproperty);


LUMIX_ENGINE_API ComponentType getComponentType(const char* id);
LUMIX_ENGINE_API u32 getComponentTypeHash(ComponentType type);
LUMIX_ENGINE_API ComponentType getComponentTypeFromHash(u32 hash);
LUMIX_ENGINE_API int getComponentTypesCount();
LUMIX_ENGINE_API const char* getComponentTypeID(int index);


namespace detail 
{


template <typename T> void writeToStream(OutputMemoryStream& stream, T value)
{
	stream.write(value);
}
	
template <typename T> T readFromStream(InputMemoryStream& stream)
{
	return stream.read<T>();
}

template <> LUMIX_ENGINE_API Path readFromStream<Path>(InputMemoryStream& stream);
template <> LUMIX_ENGINE_API void writeToStream<Path>(OutputMemoryStream& stream, Path);
template <> LUMIX_ENGINE_API void writeToStream<const Path&>(OutputMemoryStream& stream, const Path& path);
template <> LUMIX_ENGINE_API const char* readFromStream<const char*>(InputMemoryStream& stream);
template <> LUMIX_ENGINE_API void writeToStream<const char*>(OutputMemoryStream& stream, const char* path);


template <typename Getter> struct GetterProxy;

template <typename R, typename C>
struct GetterProxy<R(C::*)(EntityRef, int)>
{
	using Getter = R(C::*)(EntityRef, int);
	static void invoke(OutputMemoryStream& stream, C* inst, Getter getter, EntityRef entity, int index)
	{
		R value = (inst->*getter)(entity, index);
		writeToStream(stream, value);
	}
};

template <typename R, typename C>
struct GetterProxy<R(C::*)(EntityRef)>
{
	using Getter = R(C::*)(EntityRef);
	static void invoke(OutputMemoryStream& stream, C* inst, Getter getter, EntityRef entity, int index)
	{
		R value = (inst->*getter)(entity);
		writeToStream(stream, value);
	}
};


template <typename Setter> struct SetterProxy;

template <typename C, typename A>
struct SetterProxy<void (C::*)(EntityRef, int, A)>
{
	using Setter = void (C::*)(EntityRef, int, A);
	static void invoke(InputMemoryStream& stream, C* inst, Setter setter, EntityRef entity, int index)
	{
		using Value = RemoveCR<A>;
		auto value = readFromStream<Value>(stream);
		(inst->*setter)(entity, index, value);
	}
};

template <typename C, typename A>
struct SetterProxy<void (C::*)(EntityRef, A)>
{
	using Setter = void (C::*)(EntityRef, A);
	static void invoke(InputMemoryStream& stream, C* inst, Setter setter, EntityRef entity, int index)
	{
		using Value = RemoveCR<A>;
		auto value = readFromStream<Value>(stream);
		(inst->*setter)(entity, value);
	}
};


} // namespace detail


struct ResourceAttribute : IAttribute
{
	ResourceAttribute(const char* file_type, ResourceType type) { this->file_type = file_type; this->type = type; }
	ResourceAttribute() {}

	int getType() const override { return RESOURCE; }

	const char* file_type;
	ResourceType type;
};


struct MinAttribute : IAttribute
{
	explicit MinAttribute(float min) { this->min = min; }
	MinAttribute() {}

	int getType() const override { return MIN; }

	float min;
};


struct ClampAttribute : IAttribute
{
	ClampAttribute(float min, float max) { this->min = min; this->max = max; }
	ClampAttribute() {}

	int getType() const override { return CLAMP; }

	float min;
	float max;
};


struct RadiansAttribute : IAttribute
{
	int getType() const override { return RADIANS; }
};


struct ColorAttribute : IAttribute
{
	int getType() const override { return COLOR; }
};


template <typename T> struct Property : PropertyBase {};


struct IBlobProperty : PropertyBase {};


struct IEnumProperty : PropertyBase
{
	void visit(IAttributeVisitor& visitor) const override {}
	virtual int getEnumCount(ComponentUID cmp) const = 0;
	virtual const char* getEnumName(ComponentUID cmp, int index) const = 0;
	virtual int getEnumValueIndex(ComponentUID cmp, int value) const = 0;
	virtual int getEnumValue(ComponentUID cmp, int index) const = 0;
};


struct IArrayProperty : PropertyBase
{
	virtual bool canAddRemove() const = 0;
	virtual void addItem(ComponentUID cmp, int index) const = 0;
	virtual void removeItem(ComponentUID cmp, int index) const = 0;
	virtual int getCount(ComponentUID cmp) const = 0;
	virtual void visit(IPropertyVisitor& visitor) const = 0;
};


struct IPropertyVisitor
{
	virtual ~IPropertyVisitor() {}

	virtual void begin(const ComponentBase&) {}
	virtual void visit(const Property<float>& prop) = 0;
	virtual void visit(const Property<int>& prop) = 0;
	virtual void visit(const Property<u32>& prop) = 0;
	virtual void visit(const Property<EntityPtr>& prop) = 0;
	virtual void visit(const Property<Vec2>& prop) = 0;
	virtual void visit(const Property<Vec3>& prop) = 0;
	virtual void visit(const Property<IVec3>& prop) = 0;
	virtual void visit(const Property<Vec4>& prop) = 0;
	virtual void visit(const Property<Path>& prop) = 0;
	virtual void visit(const Property<bool>& prop) = 0;
	virtual void visit(const Property<const char*>& prop) = 0;
	virtual void visit(const IArrayProperty& prop) = 0;
	virtual void visit(const IEnumProperty& prop) = 0;
	virtual void visit(const IBlobProperty& prop) = 0;
	virtual void end(const ComponentBase&) {}
};


struct ISimpleComponentVisitor : IPropertyVisitor
{
	virtual void visitProperty(const PropertyBase& prop) = 0;

	void visit(const Property<float>& prop) override { visitProperty(prop); }
	void visit(const Property<int>& prop) override { visitProperty(prop); }
	void visit(const Property<u32>& prop) override { visitProperty(prop); }
	void visit(const Property<EntityPtr>& prop) override { visitProperty(prop); }
	void visit(const Property<Vec2>& prop) override { visitProperty(prop); }
	void visit(const Property<Vec3>& prop) override { visitProperty(prop); }
	void visit(const Property<IVec3>& prop) override { visitProperty(prop); }
	void visit(const Property<Vec4>& prop) override { visitProperty(prop); }
	void visit(const Property<Path>& prop) override { visitProperty(prop); }
	void visit(const Property<bool>& prop) override { visitProperty(prop); }
	void visit(const Property<const char*>& prop) override { visitProperty(prop); }
	void visit(const IArrayProperty& prop) override { visitProperty(prop); }
	void visit(const IEnumProperty& prop) override { visitProperty(prop); }
	void visit(const IBlobProperty& prop) override { visitProperty(prop); }
};


struct IFunctionVisitor
{
	virtual ~IFunctionVisitor() {}
	virtual void visit(const struct FunctionBase& func) = 0;
};


struct ComponentBase
{
	virtual ~ComponentBase() {}

	virtual int getPropertyCount() const = 0;
	virtual int getFunctionCount() const = 0;
	virtual void visit(IPropertyVisitor&) const = 0;
	virtual void visit(IFunctionVisitor&) const = 0;

	const char* name;
	ComponentType component_type;
};


template <typename Getter, typename Setter, typename Descriptor>
struct EnumProperty : IEnumProperty
{
	void getValue(ComponentUID cmp, int index, OutputMemoryStream& stream) const override
	{
		using C = typename ClassOf<Getter>::Type;
		C* inst = static_cast<C*>(cmp.scene);
		static_assert(4 == sizeof(typename ResultOf<Getter>::Type), "enum must have 4 bytes");
		detail::GetterProxy<Getter>::invoke(stream, inst, getter, (EntityRef)cmp.entity, index);
	}

	void setValue(ComponentUID cmp, int index, InputMemoryStream& stream) const override
	{
		using C = typename ClassOf<Getter>::Type;
		C* inst = static_cast<C*>(cmp.scene);

		static_assert(4 == sizeof(typename ResultOf<Getter>::Type), "enum must have 4 bytes");
		detail::SetterProxy<Setter>::invoke(stream, inst, setter, (EntityRef)cmp.entity, index);
	}


	int getEnumCount(ComponentUID cmp) const override
	{
		return descriptor.values_count;
	}


	const char* getEnumName(ComponentUID cmp, int index) const override
	{
		return descriptor.values[index].name;
	}


	int getEnumValue(ComponentUID cmp, int index) const override
	{
		return descriptor.values[index].value;
	}


	int getEnumValueIndex(ComponentUID cmp, int value) const override
	{
		for (int i = 0; i < descriptor.values_count; ++i)
		{
			if (descriptor.values[i].value == value) return i;
		}
		return -1;
	}


	Getter getter;
	Setter setter;
	Descriptor descriptor;
};


template <typename Getter, typename Setter, typename Counter, typename Namer>
struct DynEnumProperty : IEnumProperty
{
	void getValue(ComponentUID cmp, int index, OutputMemoryStream& stream) const override
	{
		using C = typename ClassOf<Getter>::Type;
		C* inst = static_cast<C*>(cmp.scene);
		static_assert(4 == sizeof(typename ResultOf<Getter>::Type), "enum must have 4 bytes");
		detail::GetterProxy<Getter>::invoke(stream, inst, getter, (EntityRef)cmp.entity, index);
	}

	void setValue(ComponentUID cmp, int index, InputMemoryStream& stream) const override
	{
		using C = typename ClassOf<Getter>::Type;
		C* inst = static_cast<C*>(cmp.scene);

		static_assert(4 == sizeof(typename ResultOf<Getter>::Type), "enum must have 4 bytes");
		detail::SetterProxy<Setter>::invoke(stream, inst, setter, (EntityRef)cmp.entity, index);
	}

	int getEnumValueIndex(ComponentUID cmp, int value) const override { return value; }
	int getEnumValue(ComponentUID cmp, int index) const override { return index; }

	int getEnumCount(ComponentUID cmp) const override
	{
		using C = typename ClassOf<Getter>::Type;
		C* inst = static_cast<C*>(cmp.scene);
		return (inst->*counter)();
	}


	const char* getEnumName(ComponentUID cmp, int index) const override
	{
		using C = typename ClassOf<Getter>::Type;
		C* inst = static_cast<C*>(cmp.scene);
		return (inst->*namer)(index);
	}

	Getter getter;
	Setter setter;
	Counter counter;
	Namer namer;
};


template <typename Getter, typename Setter, typename... Attributes>
struct BlobProperty : IBlobProperty
{
	void getValue(ComponentUID cmp, int index, OutputMemoryStream& stream) const override
	{
		using C = typename ClassOf<Getter>::Type;
		C* inst = static_cast<C*>(cmp.scene);
		(inst->*getter)((EntityRef)cmp.entity, stream);
	}

	void setValue(ComponentUID cmp, int index, InputMemoryStream& stream) const override
	{
		using C = typename ClassOf<Getter>::Type;
		C* inst = static_cast<C*>(cmp.scene);
		(inst->*setter)((EntityRef)cmp.entity, stream);
	}

	void visit(IAttributeVisitor& visitor) const override {
		apply([&](auto& x) { visitor.visit(x); }, attributes);
	}

	Tuple<Attributes...> attributes;
	Getter getter;
	Setter setter;
};


template <typename T, typename CmpGetter, typename PtrType, typename... Attributes>
struct VarProperty : Property<T>
{
	void visit(IAttributeVisitor& visitor) const override
	{
		apply([&](auto& x) { visitor.visit(x); }, attributes);
	}

	void getValue(ComponentUID cmp, int index, OutputMemoryStream& stream) const override
	{
		using C = typename ClassOf<CmpGetter>::Type;
		C* inst = static_cast<C*>(cmp.scene);
		auto& c = (inst->*cmp_getter)((EntityRef)cmp.entity);
		auto& v = c.*ptr;
		stream.write(v);
	}

	void setValue(ComponentUID cmp, int index, InputMemoryStream& stream) const override
	{
		using C = typename ClassOf<CmpGetter>::Type;
		C* inst = static_cast<C*>(cmp.scene);
		auto& c = (inst->*cmp_getter)((EntityRef)cmp.entity);
		stream.read(c.*ptr);
	}

	CmpGetter cmp_getter;
	PtrType ptr;
	Tuple<Attributes...> attributes;
};


template <typename T, typename Getter, typename Setter, typename... Attributes>
struct CommonProperty : Property<T>
{
	void visit(IAttributeVisitor& visitor) const override
	{
		apply([&](auto& x) { visitor.visit(x); }, attributes);
	}

	void getValue(ComponentUID cmp, int index, OutputMemoryStream& stream) const override
	{
		using C = typename ClassOf<Getter>::Type;
		C* inst = static_cast<C*>(cmp.scene);
		detail::GetterProxy<Getter>::invoke(stream, inst, getter, (EntityRef)cmp.entity, index);
	}

	void setValue(ComponentUID cmp, int index, InputMemoryStream& stream) const override
	{
		using C = typename ClassOf<Getter>::Type;
		C* inst = static_cast<C*>(cmp.scene);
		detail::SetterProxy<Setter>::invoke(stream, inst, setter, (EntityRef)cmp.entity, index);
	}


	Tuple<Attributes...> attributes;
	Getter getter;
	Setter setter;
};


template <typename Counter, typename Adder, typename Remover, typename... Props>
struct ArrayProperty : IArrayProperty
{
	ArrayProperty() {}


	bool canAddRemove() const override { return true; }


	void setValue(ComponentUID cmp, int index, InputMemoryStream& stream) const override
	{
		ASSERT(index == -1);

		int count;
		stream.read(count);
		while (getCount(cmp) < count)
		{
			addItem(cmp, -1);
		}
		while (getCount(cmp) > count)
		{
			removeItem(cmp, getCount(cmp) - 1);
		}
		for (int i = 0; i < count; ++i)
		{
			struct : ISimpleComponentVisitor
			{
				void visitProperty(const PropertyBase& prop) override
				{
					prop.setValue(cmp, index, *stream);
				}

				InputMemoryStream* stream;
				int index;
				ComponentUID cmp;

			} v;
			v.stream = &stream;
			v.index = i;
			v.cmp = cmp;
			visit(v);
		}
	}


	void getValue(ComponentUID cmp, int index, OutputMemoryStream& stream) const override
	{
		ASSERT(index == -1);
		int count = getCount(cmp);
		stream.write(count);
		for (int i = 0; i < count; ++i)
		{
			struct : ISimpleComponentVisitor
			{
				void visitProperty(const PropertyBase& prop) override
				{
					prop.getValue(cmp, index, *stream);
				}

				OutputMemoryStream* stream;
				int index;
				ComponentUID cmp;

			} v;
			v.stream = &stream;
			v.index = i;
			v.cmp = cmp;
			visit(v);
		}
	}


	void addItem(ComponentUID cmp, int index) const override
	{
		using C = typename ClassOf<Counter>::Type;
		C* inst = static_cast<C*>(cmp.scene);
		(inst->*adder)((EntityRef)cmp.entity, index);
	}


	void removeItem(ComponentUID cmp, int index) const override
	{
		using C = typename ClassOf<Counter>::Type;
		C* inst = static_cast<C*>(cmp.scene);
		(inst->*remover)((EntityRef)cmp.entity, index);
	}


	int getCount(ComponentUID cmp) const override
	{ 
		using C = typename ClassOf<Counter>::Type;
		C* inst = static_cast<C*>(cmp.scene);
		return (inst->*counter)((EntityRef)cmp.entity);
	}


	void visit(IPropertyVisitor& visitor) const override
	{
		apply([&](auto& x) { visitor.visit(x); }, properties);
	}


	void visit(IAttributeVisitor& visitor) const override {}


	Tuple<Props...> properties;
	Counter counter;
	Adder adder;
	Remover remover;
};


template <typename Counter, typename... Props>
struct ConstArrayProperty : IArrayProperty
{
	ConstArrayProperty() {}


	bool canAddRemove() const override { return false; }


	void setValue(ComponentUID cmp, int index, InputMemoryStream& stream) const override
	{
		ASSERT(index == -1);

		int count;
		stream.read(count);
		if (getCount(cmp) != count) return;
		
		for (int i = 0; i < count; ++i)
		{
			struct : ISimpleComponentVisitor
			{
				void visitProperty(const PropertyBase& prop) override
				{
					prop.setValue(cmp, index, *stream);
				}

				InputMemoryStream* stream;
				int index;
				ComponentUID cmp;

			} v;
			v.stream = &stream;
			v.index = i;
			v.cmp = cmp;
			visit(v);
		}
	}


	void getValue(ComponentUID cmp, int index, OutputMemoryStream& stream) const override
	{
		ASSERT(index == -1);
		int count = getCount(cmp);
		stream.write(count);
		for (int i = 0; i < count; ++i)
		{
			struct : ISimpleComponentVisitor
			{
				void visitProperty(const PropertyBase& prop) override
				{
					prop.getValue(cmp, index, *stream);
				}

				OutputMemoryStream* stream;
				int index;
				ComponentUID cmp;

			} v;
			v.stream = &stream;
			v.index = i;
			v.cmp = cmp;
			visit(v);
		}
	}


	void addItem(ComponentUID cmp, int index) const override { ASSERT(false); }
	void removeItem(ComponentUID cmp, int index) const override { ASSERT(false); } //-V524


	int getCount(ComponentUID cmp) const override
	{
		using C = typename ClassOf<Counter>::Type;
		C* inst = static_cast<C*>(cmp.scene);
		return (inst->*counter)((EntityRef)cmp.entity);
	}


	void visit(IPropertyVisitor& visitor) const override
	{
		apply([&](auto& x) { visitor.visit(x); }, properties);
	}


	void visit(IAttributeVisitor& visitor) const override {

	}


	Tuple<Props...> properties;
	Counter counter;
};


struct ISceneVisitor
{
	virtual ~ISceneVisitor() {}
	virtual void visit(const ComponentBase& cmp) = 0;
};

namespace internal
{
	static const unsigned int FRONT_SIZE = sizeof("Lumix::Reflection::internal::GetTypeNameHelper<") - 1u;
	static const unsigned int BACK_SIZE = sizeof(">::GetTypeName") - 1u;

	template <typename T>
	struct GetTypeNameHelper
	{
		static const char* GetTypeName()
		{
#if defined(_MSC_VER) && !defined(__clang__)
			static const size_t size = sizeof(__FUNCTION__) - FRONT_SIZE - BACK_SIZE;
			static char typeName[size] = {};
			memcpy(typeName, __FUNCTION__ + FRONT_SIZE, size - 1u); //-V594
#else
			static const size_t size = sizeof(__PRETTY_FUNCTION__) - FRONT_SIZE - BACK_SIZE;
			static char typeName[size] = {};
			memcpy(typeName, __PRETTY_FUNCTION__ + FRONT_SIZE, size - 1u);
#endif

			return typeName;
		}
	};
}


template <typename T>
const char* getTypeName()
{
	return internal::GetTypeNameHelper<T>::GetTypeName();
}


struct EnumValue
{
	EnumValue() {}
	EnumValue(const char* name, int value)
		: name(name)
		, value(value)
	{
		const char* c = name + stringLength(name);
		while (*c != ':' && c > name)
		{
			--c;
		}
		if (*c == ':') ++c;
		this->name = c;
	}

	const char* name;
	int value;
};


struct EnumBase
{
	const char* name;
	EnumValue* values;
	int values_count;
};


template <int Count>
struct EnumDescriptor : EnumBase
{
	EnumDescriptor() {}
	EnumDescriptor(const EnumDescriptor& rhs)
	{
		*this = rhs;
	}

	void operator =(const EnumDescriptor& rhs)
	{
		memcpy(this, &rhs, sizeof(*this));
		values = values_buffer;
	}

	void setValues(int) { }

	template <typename... T>
	void setValues(int i, EnumValue head, T... tail)
	{
		values_buffer[i] = head;
		values = values_buffer;
		values_count = lengthOf(values_buffer);
		setValues(i + 1, tail...);
	}

	EnumValue values_buffer[Count];
};


template <typename T, typename... Values>
auto enumDesciptor(Values... values)
{
	EnumDescriptor<sizeof...(Values)> e;
	e.name = Reflection::getTypeName<T>();
	e.setValues(0, values ...);
	return e;
}


struct SceneBase
{
	virtual ~SceneBase() {}

	virtual int getFunctionCount() const = 0;
	virtual void visit(IFunctionVisitor&) const = 0;
	virtual void visit(ISceneVisitor& visitor) const = 0;

	const char* name;
};


template <typename Components, typename Funcs>
struct Scene : SceneBase
{
	int getFunctionCount() const override { return TupleSize<Funcs>::result; }


	void visit(IFunctionVisitor& visitor) const override
	{
		apply([&](const auto& func) { visitor.visit(func); }, functions);
	}


	void visit(ISceneVisitor& visitor) const override
	{
		apply([&](const auto& cmp) { visitor.visit(cmp); }, components);
	}


	Components components;
	Funcs functions;
};


template <typename Funcs, typename Props>
struct Component : ComponentBase
{
	int getPropertyCount() const override { return TupleSize<Props>::result; }
	int getFunctionCount() const override { return TupleSize<Funcs>::result; }


	void visit(IPropertyVisitor& visitor) const override
	{
		visitor.begin(*this);
		apply([&](auto& x) { visitor.visit(x); }, properties);
		visitor.end(*this);
	}


	void visit(IFunctionVisitor& visitor) const override
	{
		apply([&](auto& x) { visitor.visit(x); }, functions);
	}


	Props properties;
	Funcs functions;
};


template <typename... Components, typename... Funcs>
auto scene(const char* name, Tuple<Funcs...> funcs, Components... components)
{
	Scene<Tuple<Components...>, Tuple<Funcs...>> scene;
	scene.name = name;
	scene.functions = funcs;
	scene.components = makeTuple(components...);
	return scene;
}


template <typename... Components>
auto scene(const char* name, Components... components)
{
	Scene<Tuple<Components...>, Tuple<>> scene;
	scene.name = name;
	scene.components = makeTuple(components...);
	return scene;
}


struct FunctionBase
{
	virtual ~FunctionBase() {}

	virtual int getArgCount() const = 0;
	virtual const char* getReturnType() const = 0;
	virtual const char* getArgType(int i) const = 0;

	const char* decl_code;
};


template <typename F> struct Function;


template <typename R, typename C, typename... Args>
struct Function<R (C::*)(Args...)> : FunctionBase
{
	using F = R(C::*)(Args...);
	F function;

	int getArgCount() const override { return sizeof...(Args); }
	const char* getReturnType() const override { return getTypeName<typename ResultOf<F>::Type>(); }
	
	const char* getArgType(int i) const override
	{
		const char* expand[] = {
			getTypeName<Args>()...
		};
		return expand[i];
	}
};


template <typename F>
auto function(F func, const char* decl_code)
{
	Function<F> ret;
	ret.function = func;
	ret.decl_code = decl_code;
	return ret;
}


template <typename... F>
auto functions(F... functions)
{
	Tuple<F...> f = makeTuple(functions...);
	return f;
}


template <typename... Props, typename... Funcs>
auto component(const char* name, Tuple<Funcs...> functions, Props... props)
{
	Component<Tuple<Funcs...>, Tuple<Props...>> cmp;
	cmp.name = name;
	cmp.functions = functions;
	cmp.properties = makeTuple(props...);
	cmp.component_type = getComponentType(name);
	return cmp;
}


template <typename... Props>
auto component(const char* name, Props... props)
{
	Component<Tuple<>, Tuple<Props...>> cmp;
	cmp.name = name;
	cmp.properties = makeTuple(props...);
	cmp.component_type = getComponentType(name);
	return cmp;
}


template <typename Getter, typename Setter, typename... Attributes>
auto blob_property(const char* name, Getter getter, Setter setter, Attributes... attributes)
{
	BlobProperty<Getter, Setter, Attributes...> p;
	p.attributes = makeTuple(attributes...);
	p.getter = getter;
	p.setter = setter;
	p.name = name;
	return p;
}


template <typename CmpGetter, typename PtrType, typename... Attributes>
auto var_property(const char* name, CmpGetter getter, PtrType ptr, Attributes... attributes)
{
	using T = typename ResultOf<PtrType>::Type;
	VarProperty<T, CmpGetter, PtrType, Attributes...> p;
	p.attributes = makeTuple(attributes...);
	p.cmp_getter = getter;
	p.ptr = ptr;
	p.name = name;
	return p;
}



template <typename Getter, typename Setter, typename... Attributes>
auto property(const char* name, Getter getter, Setter setter, Attributes... attributes)
{
	using R = typename ResultOf<Getter>::Type;
	CommonProperty<R, Getter, Setter, Attributes...> p;
	p.attributes = makeTuple(attributes...);
	p.getter = getter;
	p.setter = setter;
	p.name = name;
	return p;
}


template <typename Getter, typename Setter, typename Descriptor, typename... Attributes>
auto enum_property(const char* name, Getter getter, Setter setter, Descriptor desc, Attributes... attributes)
{
	EnumProperty<Getter, Setter, Descriptor> p;
	p.getter = getter;
	p.setter = setter;
	p.descriptor = desc;
	p.name = name;
	return p;
}


template <typename Getter, typename Setter, typename Counter, typename Namer, typename... Attributes>
auto dyn_enum_property(const char* name, Getter getter, Setter setter, Counter counter, Namer namer, Attributes... attributes)
{
	DynEnumProperty<Getter, Setter, Counter, Namer> p;
	p.getter = getter;
	p.setter = setter;
	p.namer = namer;
	p.counter = counter;
	p.name = name;
	return p;
}


template <typename Counter, typename Adder, typename Remover, typename... Props>
auto array(const char* name, Counter counter, Adder adder, Remover remover, Props... properties)
{
	ArrayProperty<Counter, Adder, Remover, Props...> p;
	p.name = name;
	p.counter = counter;
	p.adder = adder;
	p.remover = remover;
	p.properties = makeTuple(properties...);
	return p;
}


} // namespace Reflection


struct Counter {
	template <typename F>
	Counter(const F& f) {
		data = (void*)&f;
		fn = &Counter::stub<F>;
	}

	template <typename F>
	u32 stub() {
		auto& f = *(const F*)data;
		return f();
	}
	
	u32 operator()() { return (this->*fn)(); }

	void* data;
	u32 (Counter::*fn)();
};

struct Adder {
	template <typename F>
	Adder(const F& f) {
		data = (void*)&f;
		fn = &Adder::stub<F>;
	}

	template <typename F>
	void stub() {
		auto& f = *(const F*)data;
		f();
	}
	
	void operator()() { (this->*fn)(); }

	void* data;
	void (Adder::*fn)();
};

struct Remover {
	template <typename F>
	Remover(const F& f) {
		data = (void*)&f;
		fn = &Remover::stub<F>;
	}

	template <typename F>
	void stub(u32 idx) {
		auto& f = *(const F*)data;
		f(idx);
	}
	
	void operator()(u32 idx) { (this->*fn)(idx); }

	void* data;
	void (Remover::*fn)(u32);
};

template <typename T>
struct Prop {
	template <typename G, typename S>
	Prop(const char* name, const G& getter, const S& setter, Span<Reflection::IAttribute*> attrs) {
		m_getter = &getter;
		m_setter = &setter;
		m_getter_stub = &Prop::getterStub<G>;
		m_setter_stub = &Prop::setterStub<S>;
		m_attributes = attrs;
		m_name = name;
	}

	template <typename G, typename S>
	Prop(const char* name, const G& getter, const S& setter) {
		m_getter = &getter;
		m_setter = &setter;
		m_getter_stub = &Prop::getterStub<G>;
		m_setter_stub = &Prop::setterStub<S>;
		m_name = name;
	}

	Reflection::IAttribute* getAttribute(Reflection::IAttribute::Type type) const {
		for (Reflection::IAttribute* attr : m_attributes) {
			if (attr->getType() == type) return attr;
		}
		return nullptr;
	}

	template <typename S> void setterStub(T value) const { (*(const S*)m_setter)(value); }
	template <typename G> T getterStub() const { return (*(const G*)m_getter)(); }

	T get() const { return (this->*m_getter_stub)(); }
	void set(T value) const { (this->*m_setter_stub)(value); }

	const char* m_name;
	const void* m_getter;
	const void* m_setter;
	T (Prop::*m_getter_stub)() const;
	void (Prop::*m_setter_stub)(T) const;
	Span<Reflection::IAttribute*> m_attributes;
};

struct IXXVisitor {
	virtual void visit(Prop<float> prop) = 0;
	virtual void visit(Prop<bool> prop) = 0;
	virtual void visit(Prop<i32> prop) = 0;
	virtual void visit(Prop<u32> prop) = 0;
	virtual void visit(Prop<Vec2> prop) = 0;
	virtual void visit(Prop<Vec3> prop) = 0;
	virtual void visit(Prop<IVec3> prop) = 0;
	virtual void visit(Prop<Vec4> prop) = 0;
	virtual void visit(Prop<Path> prop) = 0;
	virtual void visit(Prop<EntityPtr> prop) = 0;
	virtual void visit(Prop<const char*> prop) = 0;
	
	virtual void beginArray(const char* name, Counter count, Adder add, Remover remove) = 0;
	virtual void nextArrayItem() {};
	virtual void endArray() {}
};


template <typename T, typename... Attrs>
inline void visit(IXXVisitor& visitor, const char* name, Ref<T> value, Attrs... attrs) {
	Reflection::IAttribute* attrs_array[sizeof...(attrs) + 1] = {
		&attrs...,
		nullptr
	};
	Prop<T> p(name,
		[&](){ return value.value; },
		[&](T v){ value = v; },
		Span(attrs_array, attrs_array + sizeof...(attrs)));
	visitor.visit(p);
}

template <typename C, typename G, typename S, typename... Attrs>
inline void visit(IXXVisitor& visitor, const char* name, C* inst, EntityRef e, G getter, S setter, Attrs... attrs) {
	using T = typename ResultOf<G>::Type;
	Reflection::IAttribute* attrs_array[sizeof...(attrs) + 1] = {
		&attrs...,
		nullptr
	};
	const Prop<T> p(name,
		[&](){ return (inst->*getter)(e); },
		[&](T v){ (inst->*setter)(e, v); },
		Span(attrs_array, attrs_array + sizeof...(attrs)));
	visitor.visit(p);
}

template <typename C, typename G, typename S, typename... Attrs>
inline void visit(IXXVisitor& visitor, const char* name, C* inst, EntityRef e, int idx, G getter, S setter, Attrs... attrs) {
	using T = typename ResultOf<G>::Type;
	Reflection::IAttribute* attrs_array[sizeof...(attrs) + 1] = {
		&attrs...,
		nullptr
	};
	const Prop<T> p(name,
		[&](){ return (inst->*getter)(e, idx); },
		[&](T v){ (inst->*setter)(e, idx, v); },
		Span(attrs_array, attrs_array + sizeof...(attrs)));
	visitor.visit(p);
}

inline void visit(IXXVisitor& visitor, const char* name, Ref<String> value) {
	Prop<const char*> p(name,
		[&](){ return value.value.c_str(); },
		[&](const char* v){ value = v; }
	);
	visitor.visit(p);
}


} // namespace Lumix
