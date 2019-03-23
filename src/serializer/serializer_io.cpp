#include <netlistDB/serializer/serialization_io.h>

#include <boost/filesystem/operations.hpp>

namespace netlistDB {
namespace serializer {

void iSerializationIO::file_extension(const std::string & extension) {
	FILE_EXTENSION = extension;
}
void iSerializationIO::hierarchy_push(const std::string & name) {
}
void iSerializationIO::hierarchy_pop() {
}
iSerializationIO::~iSerializationIO() {
}

SerializeToStream::SerializeToStream(std::ostream & str) :
		_str(str) {
}

std::ostream & SerializeToStream::str() {
	return _str;
}

SerializeToFiles::SerializeToFiles(const std::string & root, bool to_top_dir,
		bool flat, bool do_clear_top) :
		root(root), to_top_dir(to_top_dir), flat(flat), top_clear(false), do_clear_top(
				do_clear_top) {
	if (not boost::filesystem::is_directory(this->root))
		throw std::ios_base::failure(
				std::string("Not a accessible directory \"") + root + "\"");
}

std::ostream & SerializeToFiles::str() {
	return _str;
}

void SerializeToFiles::hierarchy_push(const std::string & name) {
	scope.push_back(name);
	boost::filesystem::path top_dir;
	if (not top_clear) {
		assert(scope.size() == 1);

		// clear or create the top directory
		if (to_top_dir) {
			top_dir = root / name;
		} else {
			top_dir = root;
		}

		if (do_clear_top) {
			boost::filesystem::remove_all(top_dir);
			boost::filesystem::create_directory(top_dir);
		} else {
			if (not boost::filesystem::is_directory(top_dir)) {
				boost::filesystem::create_directory(top_dir);
			}
		}
		top_clear = true;
	} else {
		if (flat) {
			if (to_top_dir) {
				top_dir = root / scope[0];
			} else {
				top_dir = root;
			}
		} else {
			top_dir = root;
			for (auto & p : scope) {
				top_dir = top_dir / p;
			}
		}
	}
	auto of_name = top_dir / (name + FILE_EXTENSION);
	_str.open(of_name.string());
}

void SerializeToFiles::hierarchy_pop() {
	scope.pop_back();
	_str.close();
}

SerializeToFiles::~SerializeToFiles() {
	_str.close();
}

}
}
