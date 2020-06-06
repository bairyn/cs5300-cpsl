#ifndef CPSL_CC_GRAPH_HH
#define CPSL_CC_GRAPH_HH

#include <memory>   // std::unique_ptr
#include <utility>  // std::move
#include <variant>  // std::variant

// Graph implementation based on https://hackage.haskell.org/package/algebraic-graphs .

// TODO: update Graph and LabeledGraph according to FullGraph.
template<typename T>
class Graph {
public:
	class Empty {
	public:
		Empty() {}
	};

	class Vertex {
	public:
		Vertex(T       &&vertex) : vertex(vertex) {}
		Vertex(const T  &vertex) : vertex(vertex) {}
		T vertex;
	};

	class Overlay {
	public:
		Overlay(Graph<T>       &&first,       Graph<T> &&second) : first(first), second(second) {}
		Overlay(Graph<T>       &&first, const Graph<T>  &second) : first(first), second(second) {}
		Overlay(const Graph<T>  &first,       Graph<T> &&second) : first(first), second(second) {}
		Overlay(const Graph<T>  &first, const Graph<T>  &second) : first(first), second(second) {}
		Graph<T> first;
		Graph<T> second;
	};

	class Connect {
	public:
		Connect(Graph<T>       &&first,       Graph<T> &&second) : first(first), second(second) {}
		Connect(Graph<T>       &&first, const Graph<T>  &second) : first(first), second(second) {}
		Connect(const Graph<T>  &first,       Graph<T> &&second) : first(first), second(second) {}
		Connect(const Graph<T>  &first, const Graph<T>  &second) : first(first), second(second) {}
		Graph<T> first;
		Graph<T> second;
	};

	using data_t = std::variant<
		Empty,
		Vertex,
		Overlay,
		Connect
	>;

	Graph(data_t &&data) : data(data) {}
	Graph(const data_t &data) : data(data) {}
	data_t data;

	Graph(Empty   &&empty)   : data(empty) {}
	Graph(Vertex  &&vertex)  : data(vertex) {}
	Graph(Overlay &&overlay) : data(overlay) {}
	Graph(Connect &&connect) : data(connect) {}
};

// | Graph with labeled edges.
template<typename T, typename E>
class LabeledGraph {
public:
	class Empty {
	public:
		Empty() {}
	};

	class Vertex {
	public:
		Vertex(T       &&vertex) : vertex(vertex) {}
		Vertex(const T  &vertex) : vertex(vertex) {}
		T vertex;
	};

	class Overlay {
	public:
		Overlay(LabeledGraph<T, E>       &&first,       LabeledGraph<T, E> &&second) : first(first), second(second) {}
		Overlay(LabeledGraph<T, E>       &&first, const LabeledGraph<T, E>  &second) : first(first), second(second) {}
		Overlay(const LabeledGraph<T, E>  &first,       LabeledGraph<T, E> &&second) : first(first), second(second) {}
		Overlay(const LabeledGraph<T, E>  &first, const LabeledGraph<T, E>  &second) : first(first), second(second) {}
		LabeledGraph<T, E> first;
		LabeledGraph<T, E> second;
	};

	class Connect {
	public:
		Connect(LabeledGraph<T, E>       &&first,       LabeledGraph<T, E> &&second) : label(first, second), first(first), second(second) {}
		Connect(LabeledGraph<T, E>       &&first, const LabeledGraph<T, E>  &second) : label(first, second), first(first), second(second) {}
		Connect(const LabeledGraph<T, E>  &first,       LabeledGraph<T, E> &&second) : label(first, second), first(first), second(second) {}
		Connect(const LabeledGraph<T, E>  &first, const LabeledGraph<T, E>  &second) : label(first, second), first(first), second(second) {}
		E                  label;
		LabeledGraph<T, E> first;
		LabeledGraph<T, E> second;
	};

	using data_t = std::variant<
		Empty,
		Vertex,
		Overlay,
		Connect
	>;

	LabeledGraph(data_t &&data) : data(data) {}
	LabeledGraph(const data_t &data) : data(data) {}
	data_t data;

	LabeledGraph(Empty   &&empty)   : data(empty) {}
	LabeledGraph(Vertex  &&vertex)  : data(vertex) {}
	LabeledGraph(Overlay &&overlay) : data(overlay) {}
	LabeledGraph(Connect &&connect) : data(connect) {}
};

// | Graph with labeled edges and overlays.
template<typename T, typename E, typename O>
class FullGraph {
public:
	class Empty {
	public:
		Empty() {}
	};

	class Vertex {
	public:
		Vertex(T       &&vertex) : vertex(vertex) {}
		Vertex(const T  &vertex) : vertex(vertex) {}
		T vertex;
	};

	class Overlay {
	public:
		Overlay(FullGraph<T, E, O>       &&first,       FullGraph<T, E, O> &&second) : label(*this->first, *this->second), first(new FullGraph<T, E, O>(std::move(first))), second(new FullGraph<T, E, O>(std::move(second))) {}
		Overlay(FullGraph<T, E, O>       &&first, const FullGraph<T, E, O>  &second) : label(*this->first, *this->second), first(new FullGraph<T, E, O>(std::move(first))), second(new FullGraph<T, E, O>(          second )) {}
		Overlay(const FullGraph<T, E, O>  &first,       FullGraph<T, E, O> &&second) : label(*this->first, *this->second), first(new FullGraph<T, E, O>(          first )), second(new FullGraph<T, E, O>(std::move(second))) {}
		Overlay(const FullGraph<T, E, O>  &first, const FullGraph<T, E, O>  &second) : label(*this->first, *this->second), first(new FullGraph<T, E, O>(          first )), second(new FullGraph<T, E, O>(          second )) {}
		// Put "first" and "second" first to initialize them first, so this->{first,second} is valid.
		// c.f. https://en.cppreference.com/w/cpp/language/constructor#Initialization_order
		std::unique_ptr<FullGraph<T, E, O>> first;
		std::unique_ptr<FullGraph<T, E, O>> second;
		O                                   label;

		Overlay(const Overlay &other) : label(other.label), first(new FullGraph<T, E, O>(*other.first)), second(new FullGraph<T, E, O>(*other.second)) {}
	};

	class Connect {
	public:
		Connect(FullGraph<T, E, O>       &&first,       FullGraph<T, E, O> &&second) : label(*this->first, *this->second), first(new FullGraph<T, E, O>(std::move(first))), second(new FullGraph<T, E, O>(std::move(second))) {}
		Connect(FullGraph<T, E, O>       &&first, const FullGraph<T, E, O>  &second) : label(*this->first, *this->second), first(new FullGraph<T, E, O>(std::move(first))), second(new FullGraph<T, E, O>(          second )) {}
		Connect(const FullGraph<T, E, O>  &first,       FullGraph<T, E, O> &&second) : label(*this->first, *this->second), first(new FullGraph<T, E, O>(          first )), second(new FullGraph<T, E, O>(std::move(second))) {}
		Connect(const FullGraph<T, E, O>  &first, const FullGraph<T, E, O>  &second) : label(*this->first, *this->second), first(new FullGraph<T, E, O>(          first )), second(new FullGraph<T, E, O>(          second )) {}
		std::unique_ptr<FullGraph<T, E, O>> first;
		std::unique_ptr<FullGraph<T, E, O>> second;
		E                                   label;

		Connect(const Connect &other) : label(other.label), first(new FullGraph<T, E, O>(*other.first)), second(new FullGraph<T, E, O>(*other.second)) {}
	};

	enum tag_e {
		tag_begin   = 0,
		empty_tag   = 0,
		vertex_tag  = 1,
		overlay_tag = 2,
		connect_tag = 3,
		tag_end     = 4,
	};
	typedef enum tag_e tag_t;

	using data_t = std::variant<
		Empty,
		Vertex,
		Overlay,
		Connect
	>;

	FullGraph(data_t &&data) : data(std::move(data)) {}
	FullGraph(const data_t &data) : data(data) {}
	data_t data;

	FullGraph(Empty   &&empty)   : data(std::move(empty)) {}
	FullGraph(Vertex  &&vertex)  : data(std::move(vertex)) {}
	FullGraph(Overlay &&overlay) : data(std::move(overlay)) {}
	FullGraph(Connect &&connect) : data(std::move(connect)) {}

	FullGraph(const Empty   &empty)   : data(empty) {}
	FullGraph(const Vertex  &vertex)  : data(vertex) {}
	FullGraph(const Overlay &overlay) : data(overlay) {}
	FullGraph(const Connect &connect) : data(connect) {}

	FullGraph(FullGraph &&other) : data(std::move(other.data)) {}
	FullGraph(const FullGraph &other) : data(other.data) {}
};

#endif /* #ifndef CPSL_CC_GRAPH_HH */
