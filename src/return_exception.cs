using System;

namespace Toy {
	class ReturnException : ApplicationException {
		public object result;
		public ReturnException(object value) {
			result = value;
		}
	}
}