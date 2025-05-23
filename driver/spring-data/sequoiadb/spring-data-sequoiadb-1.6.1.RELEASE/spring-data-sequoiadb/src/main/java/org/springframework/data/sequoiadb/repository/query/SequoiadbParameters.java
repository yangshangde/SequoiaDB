/*
 * Copyright 2011-2014 the original author or authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package org.springframework.data.sequoiadb.repository.query;

import java.lang.reflect.Method;
import java.util.Arrays;
import java.util.List;

import org.springframework.core.MethodParameter;
import org.springframework.data.geo.Distance;
import org.springframework.data.geo.Point;
import org.springframework.data.sequoiadb.core.query.TextCriteria;
import org.springframework.data.sequoiadb.repository.Near;
import org.springframework.data.sequoiadb.repository.query.SequoiadbParameters.SequoiadbParameter;
import org.springframework.data.repository.query.Parameter;
import org.springframework.data.repository.query.Parameters;

/**
 * Custom extension of {@link Parameters} discovering additional
 * 


 */
public class SequoiadbParameters extends Parameters<SequoiadbParameters, SequoiadbParameter> {

	private final Integer distanceIndex;
	private final Integer fullTextIndex;

	private Integer nearIndex;

	/**
	 * Creates a new {@link SequoiadbParameters} instance from the given {@link Method} and {@link SequoiadbQueryMethod}.
	 * 
	 * @param method must not be {@literal null}.
	 * @param queryMethod must not be {@literal null}.
	 */
	public SequoiadbParameters(Method method, boolean isGeoNearMethod) {

		super(method);
		List<Class<?>> parameterTypes = Arrays.asList(method.getParameterTypes());
		this.distanceIndex = parameterTypes.indexOf(Distance.class);
		this.fullTextIndex = parameterTypes.indexOf(TextCriteria.class);

		if (this.nearIndex == null && isGeoNearMethod) {
			this.nearIndex = getNearIndex(parameterTypes);
		} else if (this.nearIndex == null) {
			this.nearIndex = -1;
		}
	}

	private SequoiadbParameters(List<SequoiadbParameter> parameters, Integer distanceIndex, Integer nearIndex,
								Integer fullTextIndex) {

		super(parameters);

		this.distanceIndex = distanceIndex;
		this.nearIndex = nearIndex;
		this.fullTextIndex = fullTextIndex;
	}

	private final int getNearIndex(List<Class<?>> parameterTypes) {

		for (Class<?> reference : Arrays.asList(Point.class, double[].class)) {

			int nearIndex = parameterTypes.indexOf(reference);

			if (nearIndex == -1) {
				continue;
			}

			if (nearIndex == parameterTypes.lastIndexOf(reference)) {
				return nearIndex;
			} else {
				throw new IllegalStateException("Multiple Point parameters found but none annotated with @Near!");
			}
		}

		return -1;
	}

	/*
	 * (non-Javadoc)
	 * @see org.springframework.data.repository.query.Parameters#createParameter(org.springframework.core.MethodParameter)
	 */
	@Override
	protected SequoiadbParameter createParameter(MethodParameter parameter) {

		SequoiadbParameter sequoiadbParameter = new SequoiadbParameter(parameter);

		// Detect manually annotated @Near Point and reject multiple annotated ones
		if (this.nearIndex == null && sequoiadbParameter.isManuallyAnnotatedNearParameter()) {
			this.nearIndex = sequoiadbParameter.getIndex();
		} else if (sequoiadbParameter.isManuallyAnnotatedNearParameter()) {
			throw new IllegalStateException(String.format(
					"Found multiple @Near annotations ond method %s! Only one allowed!", parameter.getMethod().toString()));
		}

		return sequoiadbParameter;
	}

	/**
	 * Returns the index of a {@link Distance} parameter to be used for geo queries.
	 * 
	 * @return
	 */
	public int getDistanceIndex() {
		return distanceIndex;
	}

	/**
	 * Returns the index of the parameter to be used to start a geo-near query from.
	 * 
	 * @return
	 */
	public int getNearIndex() {
		return nearIndex;
	}

	/**
	 * Returns ths inde of the parameter to be used as a textquery param
	 * 
	 * @return
	 * @since 1.6
	 */
	public int getFullTextParameterIndex() {
		return fullTextIndex != null ? fullTextIndex.intValue() : -1;
	}

	/**
	 * @return
	 * @since 1.6
	 */
	public boolean hasFullTextParameter() {
		return this.fullTextIndex != null && this.fullTextIndex.intValue() >= 0;
	}

	/* 
	 * (non-Javadoc)
	 * @see org.springframework.data.repository.query.Parameters#createFrom(java.util.List)
	 */
	@Override
	protected SequoiadbParameters createFrom(List<SequoiadbParameter> parameters) {
		return new SequoiadbParameters(parameters, this.distanceIndex, this.nearIndex, this.fullTextIndex);
	}

	/**
	 * Custom {@link Parameter} implementation adding parameters of type {@link Distance} to the special ones.
	 * 

	 */
	class SequoiadbParameter extends Parameter {

		private final MethodParameter parameter;

		/**
		 * Creates a new {@link SequoiadbParameter}.
		 * 
		 * @param parameter must not be {@literal null}.
		 */
		SequoiadbParameter(MethodParameter parameter) {
			super(parameter);
			this.parameter = parameter;

			if (!isPoint() && hasNearAnnotation()) {
				throw new IllegalArgumentException("Near annotation is only allowed at Point parameter!");
			}
		}

		/*
		 * (non-Javadoc)
		 * @see org.springframework.data.repository.query.Parameter#isSpecialParameter()
		 */
		@Override
		public boolean isSpecialParameter() {
			return super.isSpecialParameter() || Distance.class.isAssignableFrom(getType()) || isNearParameter()
					|| TextCriteria.class.isAssignableFrom(getType());
		}

		private boolean isNearParameter() {
			Integer nearIndex = SequoiadbParameters.this.nearIndex;
			return nearIndex != null && nearIndex.equals(getIndex());
		}

		private boolean isManuallyAnnotatedNearParameter() {
			return isPoint() && hasNearAnnotation();
		}

		private boolean isPoint() {
			return Point.class.isAssignableFrom(getType()) || getType().equals(double[].class);
		}

		private boolean hasNearAnnotation() {
			return parameter.getParameterAnnotation(Near.class) != null;
		}

	}

}
